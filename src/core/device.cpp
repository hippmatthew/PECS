#include "src/core/include/device.hpp"

#include <iostream>
#include <queue>

#define VK_PORTABILITY_SUBSET_NAME "VK_KHR_portability_subset"

namespace vecs
{

Device::QueueFamily::QueueFamily(unsigned long index, unsigned int types)
: qf_index(index), qf_types(types)
{}

Device::QueueFamilies::QueueFamilies(const vk::raii::PhysicalDevice& vk_physicalDevice, const vk::raii::SurfaceKHR& vk_surface)
{
  std::queue<unsigned long> f_all, f_compute, f_transfer, f_async, f_sparse;

  unsigned long index = 0;
  for (const auto& family : vk_physicalDevice.getQueueFamilyProperties())
  {
    unsigned int types = 0x0000000u;

    if (family.queueFlags & vk::QueueFlagBits::eGraphics && vk_physicalDevice.getSurfaceSupportKHR(index, *vk_surface))
      types |= VECS_GRAPHICS_QUEUE_BIT | VECS_PRESENT_QUEUE_BIT;

    if (family.queueFlags & vk::QueueFlagBits::eCompute)
    {
      unsigned int type = VECS_SYNC_COMPUTE_QUEUE_BIT;
      if (!(types & VECS_GRAPHICS_QUEUE_BIT))
        type = VECS_ASYNC_COMPUTE_QUEUE_BIT;
      
      types |= type;
    }

    if (family.queueFlags & vk::QueueFlagBits::eTransfer)
    {
      unsigned int type = VECS_SYNC_TRANSFER_QUEUE_BIT;
      if (!(types & VECS_GRAPHICS_QUEUE_BIT))
        type = VECS_ASYNC_TRANSFER_QUEUE_BIT;
        
      types |= type;
    }

    if (family.queueFlags & vk::QueueFlagBits::eSparseBinding)
      types |= VECS_SPARSE_QUEUE_BIT;

    switch (to_family(types))
    {
      case All:
        f_all.emplace(index++);
        continue;
      case Compute:
        f_compute.emplace(index++);
        continue;
      case Transfer:
        f_transfer.emplace(index++);
        continue;
      case Async:
        f_async.emplace(index++);
        continue;
      case Sparse:
        f_sparse.emplace(index++);
        continue;
    }
  }

  if (!f_all.empty())
  {
    addFamily(f_all.front(), FamilyType::All);
    f_all.pop();
  }
    
  if (!f_compute.empty() && !f_transfer.empty())
  {
    addFamily(f_compute.front(), FamilyType::Compute);
    addFamily(f_transfer.front(), FamilyType::Transfer);
  }
  else if (f_async.size() >= 2)
  {
    addFamily(f_async.front(), FamilyType::Compute);
    f_async.pop();
    addFamily(f_async.front(), FamilyType::Transfer);
  }
  else if (f_all.size() >= 2)
  {
    addFamily(f_all.front(), FamilyType::Compute);
    f_all.pop();
    addFamily(f_all.front(), FamilyType::Transfer);
  }
  else if (!f_async.empty())
    addFamily(f_async.front(), FamilyType::Async);
  else if (!f_all.empty())
    addFamily(f_all.front(), FamilyType::Async);

  if (!f_sparse.empty())
    addFamily(f_sparse.front(), FamilyType::Sparse);

}

Device::QueueFamilies::~QueueFamilies()
{
  for (const auto& type : supportedFamilies)
    delete familyMap[std::to_string(type)];
}

void Device::QueueFamilies::addFamily(unsigned long index, FamilyType type)
{
  familyMap.emplace(std::pair<std::string, QueueFamily *>(
    std::to_string(type),
    new QueueFamily(index, to_bits(type))
  ));
  supportedFamilies.emplace_back(type);
}

void Device::QueueFamilies::setQueues(const vk::raii::Device& vk_device)
{
  for (const auto& type : supportedFamilies)
  {
    auto * family = familyMap[std::to_string(type)];
    family->qf_queue = vk_device.getQueue(family->qf_index, 0);
  }
}

Device::Device(const vk::raii::Instance& vk_instance, const vecs::GUI& gui)
{
  getGPU(vk_instance, gui.surface());
  createDevice();
  
  queueFamilies->setQueues(vk_device);
}

Device::~Device()
{
  delete queueFamilies;
}

const vk::raii::PhysicalDevice& Device::physical() const
{
  return vk_physicalDevice;
}

const vk::raii::Device& Device::logical() const
{
  return vk_device;
}

bool Device::hasFamily(FamilyType type) const
{
  for (const auto& familyType : queueFamilies->supportedFamilies)
  {
    if (familyType == type)
      return true;
  }

  return false;
}

unsigned long Device::familyIndex(FamilyType type) const
{
  return (queueFamilies->familyMap[std::to_string(type)])->qf_index;
}

const vk::raii::Queue& Device::queue(FamilyType type) const
{
  return (queueFamilies->familyMap[std::to_string(type)])->qf_queue;
}

void Device::getGPU(const vk::raii::Instance& vk_instance, const vk::raii::SurfaceKHR& vk_surface)
{
  std::queue<vk::raii::PhysicalDevice> discreteGPUs, integratedGPUs, virtualGPUs;

  vk::raii::PhysicalDevices GPUs(vk_instance);
  for (const auto& GPU : GPUs)
  {
    auto properties = GPU.getProperties();
    QueueFamilies families(GPU, vk_surface);

    bool hasAllFamily = false;
    for (const auto& family : families.supportedFamilies)
    {
      if (family == FamilyType::All)
      {
        hasAllFamily = true;
        break;
      }
    }
    if (!hasAllFamily) continue;

    bool supportsExtensions;
    for (const auto& extension : Settings::instance().device_extensions())
    {
      supportsExtensions = false;
      for (const auto& property : GPU.enumerateDeviceExtensionProperties())
      {
        if (std::string(property.extensionName) == extension)
        {
          supportsExtensions = true;
          break;
        }
      }

      if (!supportsExtensions) break;
    }
    if (!supportsExtensions) continue;

    if (GPU.getSurfaceFormatsKHR(*vk_surface).empty()) continue;
    if (GPU.getSurfacePresentModesKHR(*vk_surface).empty()) continue;

    switch (properties.deviceType)
    {
      case vk::PhysicalDeviceType::eDiscreteGpu:
        discreteGPUs.emplace(GPU);
      case vk::PhysicalDeviceType::eIntegratedGpu:
        integratedGPUs.emplace(GPU);
      case vk::PhysicalDeviceType::eVirtualGpu:
        virtualGPUs.emplace(GPU);
      default:
        continue;
    }
  }

  if (!discreteGPUs.empty()) vk_physicalDevice = discreteGPUs.front();
  else if (!integratedGPUs.empty()) vk_physicalDevice = integratedGPUs.front();
  else if (!virtualGPUs.empty()) vk_physicalDevice = virtualGPUs.front();
  else throw std::runtime_error("error @ vecs::Device::getGPU() : no suitable gpu found");

  queueFamilies = new QueueFamilies(vk_physicalDevice, vk_surface);
}

void Device::createDevice()
{  
  float queuePriority = 1.0f;
  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

  for (const auto& type : queueFamilies->supportedFamilies)
  {
    auto * family = queueFamilies->familyMap[std::to_string(type)];

    vk::DeviceQueueCreateInfo createInfo{
      .queueFamilyIndex = static_cast<unsigned int>(family->qf_index),
      .queueCount       = 1,
      .pQueuePriorities = &queuePriority
    };

    queueCreateInfos.emplace_back(createInfo);
  }

  vk::PhysicalDeviceFeatures features{};

  if (Settings::instance().portability_enabled())
    Settings::instance().add_device_extension(VK_PORTABILITY_SUBSET_NAME);
    

  vk::PhysicalDeviceDynamicRenderingFeatures dynamicRendering{
    .dynamicRendering = true
  };

  std::vector<const char *> extensions = Settings::instance().device_extensions();
  vk::DeviceCreateInfo ci_device{
    .pNext                    = &dynamicRendering,
    .queueCreateInfoCount     = static_cast<unsigned int>(queueCreateInfos.size()),
    .pQueueCreateInfos        = queueCreateInfos.data(),
    .enabledExtensionCount    = static_cast<unsigned int>(extensions.size()),
    .ppEnabledExtensionNames  = extensions.data(),
    .pEnabledFeatures         = &features
  };
  
  vk_device = vk_physicalDevice.createDevice(ci_device);
}

unsigned int to_bits(FamilyType type)
{
  switch (type)
  {
    case All:
      return VECS_ALL_FAMILY;
    case Compute:
      return VECS_COMPUTE_FAMILY;
    case Transfer:
      return VECS_TRANSFER_FAMILY;
    case Async:
      return VECS_ASYNC_FAMILY;
    case Sparse:
      return VECS_SPARSE_FAMILY;
  }
}

FamilyType to_family(unsigned int bits)
{
  switch (bits)
  {
    case VECS_ALL_FAMILY:
      return FamilyType::All;
    case VECS_COMPUTE_FAMILY:
      return FamilyType::Compute;
    case VECS_TRANSFER_FAMILY:
      return FamilyType::Transfer;
    case VECS_ASYNC_FAMILY:
      return FamilyType::Async;
    case VECS_SPARSE_FAMILY:
      return FamilyType::Sparse;
    default:
      throw std::runtime_error("error @ vecs::to_family() : bits did not correspond to a family type");
  }
}

} // namespace vecs

namespace std
{

std::string to_string(vecs::FamilyType type)
{
  switch (type)
  {
    case vecs::FamilyType::All:
      return "All";
    case vecs::FamilyType::Compute:
      return "Compute";
    case vecs::FamilyType::Transfer:
      return "Transfer";
    case vecs::FamilyType::Async:
      return "Async";
    case vecs::FamilyType::Sparse:
      return "Sparse";
  }
}

} // namespace std
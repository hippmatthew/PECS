#include "src/core/include/device.hpp"

#include <queue>

namespace std
{

std::string to_string(pecs::FamilyType type)
{
  switch (type)
  {
    case pecs::FamilyType::AllQueue:
      return "AllQueue";
    case pecs::FamilyType::Compute:
      return "AsyncCompute";
    case pecs::FamilyType::Transfer:
      return "AsyncTransfer";
    case pecs::FamilyType::Sparse:
      return "Sparse";
    default:
      return "NoFamily";
  }
}

} // namespace std

namespace pecs
{

FamilyType to_family(unsigned int bits)
{
  
  if ((bits & PECS_ALLQUEUE_FAMILY) == (PECS_ALLQUEUE_FAMILY))
    return FamilyType::AllQueue;

  if ((bits & PECS_ASYNC_COMBINE_FAMILY) == (PECS_ASYNC_COMBINE_FAMILY))
    return FamilyType::AsyncCombine;

  if (bits & PECS_SPARSE_FAMILY)
    return FamilyType::Sparse;

  if ((bits & PECS_ASYNC_COMPUTE_FAMILY) == (PECS_ASYNC_COMPUTE_FAMILY))
    return FamilyType::Compute;

  if ((bits & PECS_ASYNC_TRANSFER_FAMILY) == (PECS_ASYNC_TRANSFER_FAMILY))
    return FamilyType::Transfer;
  
  return FamilyType::NoFamily;
}

Device::Device(const vk::raii::Instance& vk_instance, const vk::raii::SurfaceKHR& vk_surface)
{
  vk_physicalDevice = bestPhysicalDevice(vk_instance, vk_surface);
  queueFamilies = new QueueFamilies(vk_physicalDevice, vk_surface);
  createLogicalDevice();
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
  for (const auto& familyType : queueFamilies->familyTypes)
  {
    if (familyType == type)
      return true;
  }

  return false;
}

const unsigned long Device::familyIndex(FamilyType type) const
{
  return queueFamilies->familyMap.at(std::to_string(type))->qf_index;
}

const vk::raii::Queue& Device::queue(FamilyType type) const
{
  auto * family = queueFamilies->familyMap.at(std::to_string(type));
  return family->qf_queue;
}

bool Device::supportsExtensions(const vk::raii::PhysicalDevice& physicalDevice) const
{
  for (const auto& extension : extensions)
  {
    bool found = false;
    for (const auto& property : physicalDevice.enumerateDeviceExtensionProperties())
    {
      if (std::string(property.extensionName) == extension)
      {
        found = true;
        break;
      } 
    }

    if (!found) return false;
  }

  return true;
}

vk::raii::PhysicalDevice Device::bestPhysicalDevice(const vk::raii::Instance& vk_instance, const vk::raii::SurfaceKHR& vk_surface)
{
  std::queue<vk::raii::PhysicalDevice> discreteGPUs, integratedGPUs, virtualGPUs;
  
  vk::raii::PhysicalDevices physicalDevices(vk_instance);
  for (const auto& physicalDevice : physicalDevices)
  {
    auto properties = physicalDevice.getProperties();
    QueueFamilies families(physicalDevice, vk_surface);

    bool hasAllQueue = false;
    for (const auto& familyType : families.familyTypes)
    {
      if (familyType == AllQueue)
      {
        hasAllQueue = true;
        break;
      }
    }

    if (!hasAllQueue) continue;
    if (!supportsExtensions(physicalDevice)) continue;
    if (physicalDevice.getSurfaceFormatsKHR(*vk_surface).empty()) continue;
    if (physicalDevice.getSurfacePresentModesKHR(*vk_surface).empty()) continue;

    switch(properties.deviceType)
    {
      case vk::PhysicalDeviceType::eDiscreteGpu:
        discreteGPUs.emplace(physicalDevice);
        break;
      case vk::PhysicalDeviceType::eIntegratedGpu:
        integratedGPUs.emplace(physicalDevice);
        break;
      case vk::PhysicalDeviceType::eVirtualGpu:
        virtualGPUs.emplace(physicalDevice);
        break;
      default:
        continue;
    }
  }

  if (!discreteGPUs.empty()) return discreteGPUs.front();
  if (!integratedGPUs.empty()) return integratedGPUs.front();
  if (!virtualGPUs.empty()) return virtualGPUs.front();

  throw std::runtime_error("pecs : error finding physical device - no suitable device found");
}

void Device::createLogicalDevice()
{
  float queuePriority = 1.0f;
  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

  for (const auto& familyType : queueFamilies->familyTypes)
  {
    auto * family = queueFamilies->familyMap.at(std::to_string(familyType));
    
    vk::DeviceQueueCreateInfo createInfo{
      .queueFamilyIndex = static_cast<unsigned int>(family->qf_index),
      .queueCount       = 1,
      .pQueuePriorities = &queuePriority
    };

    queueCreateInfos.emplace_back(createInfo);
  }

  vk::PhysicalDeviceFeatures physicalDeviceFeatures{};

  for (const auto& extension : vk_physicalDevice.enumerateDeviceExtensionProperties())
  {
    if (std::string(extension.extensionName) == VK_KHR_PORTABILITY_SUBSET_NAME)
    {
      extensions.emplace_back(VK_KHR_PORTABILITY_SUBSET_NAME);
      break;
    }
  }

  vk::PhysicalDeviceDynamicRenderingFeatures dynamicRendering{
    .dynamicRendering = vk::True
  };

  vk::DeviceCreateInfo ci_device{
    .pNext                    = &dynamicRendering,
    .queueCreateInfoCount     = static_cast<unsigned int>(queueCreateInfos.size()),
    .pQueueCreateInfos        = queueCreateInfos.data(),
    .enabledExtensionCount    = static_cast<unsigned int>(extensions.size()),
    .ppEnabledExtensionNames  = extensions.data(),
    .pEnabledFeatures         = &physicalDeviceFeatures,
  };

  vk_device = vk_physicalDevice.createDevice(ci_device);
}

} // namespace pecs
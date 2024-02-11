/*
 *  PECS::core - device.cpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  2/8/24
 */

#include "src/core/include/device.hpp"

#include <queue>

namespace pecs
{

Device::Device(const vk::raii::Instance& vk_instance, const vk::raii::SurfaceKHR& vk_surface)
{
  vk_physicalDevice = bestPhysicalDevice(vk_instance, vk_surface);
  queues = new Queues(vk_physicalDevice, vk_surface);
  createLogicalDevice();
  queues->setQueues(vk_device);
}

Device::~Device()
{
  delete queues;
}

const vk::raii::PhysicalDevice& Device::physical() const
{
  return vk_physicalDevice;
}

const vk::raii::Device& Device::logical() const
{
  return vk_device;
}

const std::vector<unsigned int> Device::queueFamilyArray() const
{
  return queues->array();
}

const vk::raii::Queue& Device::queue(QueueType type) const
{
  return queues->queue(type);
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
    Queues queueFamilies(physicalDevice, vk_surface);

    if (!queueFamilies.isComplete()) continue;
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

  for (unsigned int index : queues->array())
  {
    vk::DeviceQueueCreateInfo createInfo{
      .queueFamilyIndex = index,
      .queueCount       = 1,
      .pQueuePriorities = &queuePriority
    };

    queueCreateInfos.emplace_back(createInfo);
  }

  vk::PhysicalDeviceFeatures physicalDeviceFeatures{};

  for (const auto& extension : vk_physicalDevice.enumerateDeviceExtensionProperties())
  {
    if (std::string(extension.extensionName) == "VK_KHR_portability_subset")
    {
      extensions.emplace_back("VK_KHR_portability_subset");
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
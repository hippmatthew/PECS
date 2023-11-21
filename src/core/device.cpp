/*
 *  PECS - device.cpp
 *  Author:     Matthew Hipp
 *  Created:    7/21/23
 *  Updated:    10/29/23
 */

#include "include/device.hpp"

namespace pecs
{

Device::Device(std::vector<vk::PhysicalDevice> devices, const vk::SurfaceKHR& surface)
{
    std::cout << "\tpicking physical device...\n";
    choosePhysicalDevice(devices, surface);

    std::cout << "\tcreating logical device...\n";
    createLogicalDevice(surface);
}

Device::~Device()
{
    std::cout << "\tdestroying logical device...\n";
    logicalDevice.destroy();
}

void Device::choosePhysicalDevice(std::vector<vk::PhysicalDevice> devices, const vk::SurfaceKHR& surface)
{
    auto suitableDevices = getSuitablePhysicalDevices(devices, surface);
    if (suitableDevices.size() == 0) throw std::runtime_error("no suitable deviced found\n");

    for (const auto& device : suitableDevices)
    {
        auto properties = device.getProperties();
        std::cout << "\t\t\tname: " << static_cast<std::string>(properties.deviceName)  << "\ttype: "  << vk::to_string(properties.deviceType) << '\n';
    }
    
    physicalDevice = suitableDevices[0];

    auto properties = physicalDevice.getProperties();
    std::cout << "\t\tchosen physical device: " << static_cast<std::string>(properties.deviceName) << '\n';
    std::cout << "\t\tavailable physical device extensions:\n";

    auto extensionProperties = physicalDevice.enumerateDeviceExtensionProperties();
    for (const auto& property : extensionProperties)
    {
        std::cout << "\t\t\t" << static_cast<std::string>(property.extensionName);
            
        for (const auto& extension : deviceExtensions)
        {
            if (strcmp(extension, property.extensionName) == 0)
            {
                std::cout << " (required)";
                break;
            }
        }

        std::cout << '\n';  
    }
}

void Device::createLogicalDevice(const vk::SurfaceKHR& surface)
{
    QueueFamilyIndices indices = findPhysicalDeviceQueueFamilyIndicies(physicalDevice, surface);

    float graphicsQueuePriority = 1.0f;
    vk::DeviceQueueCreateInfo graphicsQueueCreateInfo{ .queueFamilyIndex    = indices.graphics.value(),
                                                       .queueCount          = 1,
                                                       .pQueuePriorities    = &graphicsQueuePriority };

    float presentQueuePriority = 1.0f;
    vk::DeviceQueueCreateInfo presentQueueCreateInfo{ .queueFamilyIndex = indices.present.value(),
                                                      .queueCount       = 1,
                                                      .pQueuePriorities = &presentQueuePriority };

    float computeQueuePriority = 1.0f;
    vk::DeviceQueueCreateInfo computeQueueCreateInfo{ .queueFamilyIndex = indices.compute.value(),
                                                      .queueCount       = 1,
                                                      .pQueuePriorities = &computeQueuePriority };

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos = { graphicsQueueCreateInfo, presentQueueCreateInfo, computeQueueCreateInfo };

    vk::PhysicalDeviceFeatures physicalDeviceFeatures{};

    vk::DeviceCreateInfo deviceCreateInfo{ .queueCreateInfoCount    = static_cast<unsigned int>(queueCreateInfos.size()),
                                           .pQueueCreateInfos       = queueCreateInfos.data(),
                                           .enabledLayerCount       = 0,
                                           .enabledExtensionCount   = static_cast<unsigned int>(deviceExtensions.size()),
                                           .ppEnabledExtensionNames = deviceExtensions.data(),
                                           .pEnabledFeatures        = &physicalDeviceFeatures };

    logicalDevice = physicalDevice.createDevice(deviceCreateInfo);
    std::cout << "\t\tcreated logical device\n";
    
    logicalDevice.getQueue(indices.graphics.value(), 0, &graphicsQueue);
    std::cout << "\t\testablished graphics queue\n";

    logicalDevice.getQueue(indices.present.value(), 0, &presentQueue);
    std::cout << "\t\testablished present queue\n";

    logicalDevice.getQueue(indices.compute.value(), 0, &computeQueue);
    std::cout << "\t\testablished compute queue\n";
}

std::vector<vk::PhysicalDevice> Device::getSuitablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices, const vk::SurfaceKHR& surface) const
{
    std::vector<vk::PhysicalDevice> discreteGpus, integratedGpus, virtualGpus, sortedGpus;

    for (auto& device : devices)
    {
        auto properties = device.getProperties();

        QueueFamilyIndices indices = findPhysicalDeviceQueueFamilyIndicies(device, surface);
        if (!indices.isComplete()) continue;

        if (!checkPhysicalDeviceExtensionSupport(device)) continue;

        auto swapchainSupport = querySwapchainSupport(device, surface);
        
        if (swapchainSupport.formats.empty()) continue;
        if (swapchainSupport.presentModes.empty()) continue;

        switch (evaluate(properties.deviceType))
        {
            case 3:
                discreteGpus.push_back(device);
                continue;
            case 2:
                integratedGpus.push_back(device);
                continue;
            case 1:
                virtualGpus.push_back(device);
                continue;
            case 0:
                continue;
        }
    }
    
    for (const auto& device : discreteGpus)
        sortedGpus.push_back(device);

    for (const auto& device : integratedGpus)
        sortedGpus.push_back(device);

    for (const auto& device : virtualGpus)
        sortedGpus.push_back(device);

    return sortedGpus;
}

QueueFamilyIndices Device::findPhysicalDeviceQueueFamilyIndicies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) const
{
    QueueFamilyIndices indices;

    auto queueFamilies = device.getQueueFamilyProperties();
    
    unsigned int i = 0;
    for (auto family : queueFamilies)
    {   
        if (family.queueFlags & vk::QueueFlagBits::eGraphics)
            indices.graphics = i;
        
        if (family.queueFlags & vk::QueueFlagBits::eCompute)
            indices.compute = i;

        if (indices.isComplete())
            break;

        vk::Bool32 presentQueueSupport = false;
        vk::Result result = device.getSurfaceSupportKHR(i, surface, &presentQueueSupport);
        if (result != vk::Result::eSuccess) throw std::runtime_error(vk::to_string(result));

        if (presentQueueSupport)
            indices.present = i;

        ++i;
    }

    return indices;
}

unsigned int Device::evaluate(const vk::PhysicalDeviceType type) const
{
    switch (type)
    {
        case vk::PhysicalDeviceType::eDiscreteGpu:
            return 3;
        case vk::PhysicalDeviceType::eIntegratedGpu:
            return 2;
        case vk::PhysicalDeviceType::eVirtualGpu:
            return 1;
        default:
            return 0;
    }
}

bool Device::checkPhysicalDeviceExtensionSupport(const vk::PhysicalDevice& device) const
{
    auto extensionProperties = device.enumerateDeviceExtensionProperties();
    
    std::vector<bool> supported;
    for (const auto& extension : deviceExtensions)
    {
        bool support = false;
        
        for (const auto& property : extensionProperties)
            if (strcmp(property.extensionName, extension) == 0)
                support = true;

        supported.push_back(support);
    }

    for (const auto& support : supported)
        if (!support) return false;

    return true;
}

SwapchainSupportDetails Device::querySwapchainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) const
{
    SwapchainSupportDetails details{ .surfaceCapabilities   = device.getSurfaceCapabilitiesKHR(surface),
                                     .formats               = device.getSurfaceFormatsKHR(surface),
                                     .presentModes          = device.getSurfacePresentModesKHR(surface) };

    return details;
}

}
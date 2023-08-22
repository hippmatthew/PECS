/*
 *  PECS - device.cpp
 *  Author:     Matthew Hipp
 *  Created:    7/21/23
 *  Updated:    7/22/23
 */

#include "include/device.hpp"

namespace pecs
{

Device::Device(const vk::Instance& instance, const DebugManager * dm) : debugManager(dm)
{
    if (debugManager->isEnabled()) debugManager->message("picking physical device...");
    choosePhysicalDevice(instance);
}

vk::PhysicalDeviceProperties Device::getPhysicalDeviceProperties() const
{ return physicalDevice.getProperties(); }

void Device::choosePhysicalDevice(const vk::Instance& instance)
{
    unsigned int deviceCount;
    static_cast<void>(instance.enumeratePhysicalDevices(&deviceCount, nullptr));

    if (deviceCount == 0) debugManager->message("no physical devices found", true);

    std::vector<vk::PhysicalDevice> devices(deviceCount);
    vk::Result result = instance.enumeratePhysicalDevices(&deviceCount, devices.data());

    switch(result)
    {
        case vk::Result::eSuccess:
            if (debugManager->isEnabled()) debugManager->message("\tavailable devices:");
            break;
        case vk::Result::eIncomplete:
            if (debugManager->isEnabled()) debugManager->message("\twarning: not all devices accounted for\n\tavailable devices:");
            break;
        default:
            debugManager->message(result);
    }

    auto suitableDevices = getSuitablePhysicalDevices(devices);
    if (suitableDevices.size() == 0) debugManager->message("no suitable physical devices found", true);

    if (debugManager->isEnabled())
    {
        for (const auto& device : suitableDevices)
        {
            auto properties = device.getProperties();
            debugManager->message("\t\t" + static_cast<std::string>(properties.deviceName) + " : " + vkPhysicalDeviceTypeToString(properties.deviceType));
        }
    }
    
    physicalDevice = suitableDevices[0];
    
    if (debugManager->isEnabled())
    {
        auto properties = physicalDevice.getProperties();
        debugManager->message("\tchosen physical device: " + static_cast<std::string>(properties.deviceName));
    }
}

std::vector<vk::PhysicalDevice> Device::getSuitablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices) const
{
    std::vector<vk::PhysicalDevice> discreteGpus, integratedGpus, virtualGpus, sortedGpus;

    for (auto& device : devices)
    {
        auto properties = device.getProperties();
        auto queueFamilies = device.getQueueFamilyProperties();

        QueueFamilyIndices indices = findPhysicalDeviceQueueFamilyIndicies(device, queueFamilies);
        if (!indices.isComplete()) continue;

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

QueueFamilyIndices Device::findPhysicalDeviceQueueFamilyIndicies(const vk::PhysicalDevice& device, const std::vector<vk::QueueFamilyProperties>& queueFamilies) const
{
    QueueFamilyIndices indices;
    
    unsigned int i = 0;
    for (auto family : queueFamilies)
    {   
        if (family.queueFlags & vk::QueueFlagBits::eGraphics)
            indices.graphics = i;
        
        if (family.queueFlags & vk::QueueFlagBits::eCompute)
            indices.compute = i;

        if (indices.isComplete())
            break;

        ++i;
    }

    return indices;
}

unsigned int Device::evaluate(vk::PhysicalDeviceType type) const
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

std::string Device::vkPhysicalDeviceTypeToString(const vk::PhysicalDeviceType type) const
{
    const char * cString = string_VkPhysicalDeviceType(static_cast<VkPhysicalDeviceType>(type));
    return static_cast<std::string>(cString);
}

}
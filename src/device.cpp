/*
 *  PECS - device.cpp
 *  Author:     Matthew Hipp
 *  Created:    7/21/23
 *  Updated:    7/21/23
 */

#include "include/device.hpp"

namespace pecs
{

Device::Device(const vk::Instance& instance, const DebugManager * dm) : debugManager(dm)
{
    if (debugManager->isEnabled()) debugManager->message("picking physical device...");
    choosePhysicalDevice(instance);
}

Device::~Device()
{

}

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
        case vk::Result::eIncomplete:
            if (debugManager->isEnabled()) debugManager->message("\twarning: not all devices accounted for\n\tavailable devices:");
        default:
            debugManager->message(result);
    }

}

std::multimap<vk::PhysicalDeviceType, vk::PhysicalDevice> Device::getSuitablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices) const
{
    std::multimap<vk::PhysicalDeviceType, vk::PhysicalDevice> suitableDevices;

    for (auto& device : devices)
    {
        auto properties = device.getProperties();
        auto queueFamilies = device.getQueueFamilyProperties();

        QueueFamilyIndices indices = findPhysicalDeviceQueueFamilyIndicies(device, queueFamilies);
        if (!indices.isComplete()) continue;

        // Not a good solution. Only good for people with singular gpu types (i.e 1 discrete, 1 integrated, 1 other, etc.)
        // Come up with a better solution
        
        suitableDevices.insert(std::make_pair(properties.deviceType, device));
    }
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

        ++i;
    }

    return indices;
}

}
/*
 *  PECS - device.hpp
 *  Author:     Matthew Hipp
 *  Created:    7/21/23
 *  Updated:    7/21/23
 */

#ifndef pecs_device_hpp
#define pecs_device_hpp

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <vector>
#include <map>
#include <optional>

#include "debug.hpp"

namespace pecs
{

struct QueueFamilyIndices
{
    std::optional<unsigned int> graphics;
    std::optional<unsigned int> compute;

    bool isComplete() { return graphics.has_value() && compute.has_value(); }
};

class Device
{
    public:
        Device(const vk::Instance& instance, const DebugManager * dm);
        Device(const Device&) = delete;

        ~Device();

        Device& operator=(const Device&) = delete;

    private:
        const DebugManager * debugManager;
        
        vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
        vk::Device logicalDevice = VK_NULL_HANDLE;

        void choosePhysicalDevice(const vk::Instance& instance);
        
        std::multimap<vk::PhysicalDeviceType, vk::PhysicalDevice> getSuitablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices) const;
        QueueFamilyIndices findPhysicalDeviceQueueFamilyIndicies(const vk::PhysicalDevice& device, const  std::vector<vk::QueueFamilyProperties>& queueFamilies) const;
};

}

#endif /* pecs_device_hpp */
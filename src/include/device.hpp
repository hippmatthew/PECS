/*
 *  PECS - device.hpp
 *  Author:     Matthew Hipp
 *  Created:    7/21/23
 *  Updated:    7/23/23
 */

#ifndef pecs_device_hpp
#define pecs_device_hpp

#include <vector>
#include <optional>

#include "debug.hpp"

namespace pecs
{

enum QueueType
{
    Graphics,
    Present,
    Compute
};

struct QueueFamilyIndices
{
    std::optional<unsigned int> graphics;
    std::optional<unsigned int> present;
    std::optional<unsigned int> compute;

    bool isComplete() { return graphics.has_value() && present.has_value() && compute.has_value(); }
};

class Device
{
    public:
        Device(const vk::Instance& instance, const vk::SurfaceKHR& surface, const DebugManager * dm);
        Device(const Device&) = delete;

        ~Device();

        Device& operator=(const Device&) = delete;

        vk::PhysicalDeviceProperties getPhysicalDeviceProperties() const;
        const vk::Queue& getQueue(QueueType type) const;

    private:
        const DebugManager * debugManager;
        
        vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
        vk::Device logicalDevice = VK_NULL_HANDLE;

        vk::Queue graphicsQueue = VK_NULL_HANDLE;
        vk::Queue presentQueue = VK_NULL_HANDLE;
        vk::Queue computeQueue = VK_NULL_HANDLE;

        void choosePhysicalDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface);
        void createLogicalDevice(const vk::SurfaceKHR& surface);
        
        std::vector<vk::PhysicalDevice> getSuitablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices, const vk::SurfaceKHR& surface) const;
        QueueFamilyIndices findPhysicalDeviceQueueFamilyIndicies(const vk::PhysicalDevice& device, const  std::vector<vk::QueueFamilyProperties>& queueFamilies, const vk::SurfaceKHR& surface) const;
        unsigned int evaluate(const vk::PhysicalDeviceType type) const;
};

}

#endif /* pecs_device_hpp */
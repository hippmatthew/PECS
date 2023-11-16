/*
 *  PECS - device.hpp
 *  Author:     Matthew Hipp
 *  Created:    7/21/23
 *  Updated:    10/29/23
 */

#ifndef pecs_device_hpp
#define pecs_device_hpp

#include <algorithm>
#include <limits>
#include <optional>
#include <vector>

#include "window.hpp"

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

    bool isComplete() const
    { return graphics.has_value() && present.has_value() && compute.has_value(); }
};

struct SwapchainSupportDetails
{
    vk::SurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

class Device
{
    public:
        Device(const vk::Instance& instance, const Window * window);
        Device(const Device&) = delete;
        Device(Device&&) = delete;

        ~Device();

        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&) = delete;

        const vk::PhysicalDevice& getPhysicalDevice() const;
        vk::PhysicalDeviceProperties getPhysicalDeviceProperties() const;
        const vk::Device& getLogicalDevice() const;
        const vk::Queue& getQueue(QueueType type) const;
        SwapchainSupportDetails querySwapchainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) const;
        QueueFamilyIndices findPhysicalDeviceQueueFamilyIndicies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) const;

    private:
        // required device extensions
        const std::vector<const char *> deviceExtensions = { 
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
        };
        
        vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
        vk::Device logicalDevice = VK_NULL_HANDLE;

        vk::Queue graphicsQueue = VK_NULL_HANDLE;
        vk::Queue presentQueue = VK_NULL_HANDLE;
        vk::Queue computeQueue = VK_NULL_HANDLE;

        void choosePhysicalDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface);
        void createLogicalDevice(const vk::SurfaceKHR& surface);
        
        std::vector<vk::PhysicalDevice> getSuitablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices, const vk::SurfaceKHR& surface) const;
        unsigned int evaluate(const vk::PhysicalDeviceType type) const;
        bool checkPhysicalDeviceExtensionSupport(const vk::PhysicalDevice& device) const;  
};

}

#endif /* pecs_device_hpp */
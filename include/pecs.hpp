/*
*   PECS - pecs.hpp
*   Author:     Matthew Hipp
*   Created:    6/29/23
*   Updated:    7/25/23
*/

#ifndef pecs_hpp
#define pecs_hpp

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <optional>
#include <algorithm>
#include <limits>

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_to_string.hpp>
    
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace pecs
{

enum QueueType
{
    Graphics,
    Present,
    Compute
};

struct InitializationInfo
{
    std::string engineName = "PECS";
    unsigned int engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    
    std::string applicationName = "PECS Application";
    unsigned int applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

    std::string windowTitle = "PECS Application";
    unsigned int windowWidth = 600, windowHeight = 600;

    bool enableDebugManager = true;
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

class DebugManager
{
    public:            
        DebugManager(bool enable = true) : enabled(enable) {};
        DebugManager(const DebugManager&) = delete;
            
        ~DebugManager() = default;

        DebugManager operator=(const DebugManager&) = delete;

        bool isEnabled() const;

        void message(const std::string s, bool err = false) const;
        void message(const vk::Result r) const;

    private:
        const bool enabled;
};

class Window
{
    public:
        Window(unsigned int width, unsigned int height, std::string title, const DebugManager * dm);
        Window(const Window&) = delete;
        
        ~Window();

        Window& operator=(const Window&) = delete;
        
        const GLFWwindow * getGLFWwindow() const;
        const vk::SurfaceKHR& getSurface() const;
        
        bool shouldClose() const;

        void createSurface(const vk::Instance& instance);
        void destroySurface(const vk::Instance& instance);

    private:
        const DebugManager * debugManager;
        
        GLFWwindow * window = nullptr;
        vk::SurfaceKHR surface;
};

class Device
{
    public:
        Device(const vk::Instance& instance, const Window * window, const DebugManager * dm);
        Device(const Device&) = delete;

        ~Device();

        Device& operator=(const Device&) = delete;

        const vk::PhysicalDevice& getPhysicalDevice() const;
        vk::PhysicalDeviceProperties getPhysicalDeviceProperties() const;
        const vk::Queue& getQueue(QueueType type) const;

    private:
        const DebugManager * debugManager;
        const std::vector<const char *> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        
        vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
        vk::Device logicalDevice = VK_NULL_HANDLE;
        vk::SwapchainKHR swapchain = VK_NULL_HANDLE;

        vk::Queue graphicsQueue = VK_NULL_HANDLE;
        vk::Queue presentQueue = VK_NULL_HANDLE;
        vk::Queue computeQueue = VK_NULL_HANDLE;

        void choosePhysicalDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface);
        void createLogicalDevice(const vk::SurfaceKHR& surface);
        void createSwapchain(const Window * window);
        
        std::vector<vk::PhysicalDevice> getSuitablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices, const vk::SurfaceKHR& surface) const;
        QueueFamilyIndices findPhysicalDeviceQueueFamilyIndicies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) const;
        unsigned int evaluate(const vk::PhysicalDeviceType type) const;
        bool checkPhysicalDeviceExtensionSupport(const vk::PhysicalDevice& device) const;
        
        SwapchainSupportDetails querySwapchainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) const;
        vk::SurfaceFormatKHR chooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const;
        vk::PresentModeKHR chooseSwapchainPresentMode(const std::vector<vk::PresentModeKHR>& availableModes) const;
        vk::Extent2D chooseSwapchainExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const GLFWwindow * window) const;
};

class Engine
{
    public:
        Engine() = default;
        Engine(const Engine&) = delete;
        ~Engine();

        Engine& operator=(const Engine&) = delete;

        bool isActive() const;
        void getEvents() const;

        void initialize(const InitializationInfo* initInfo);

    private:
        Window * window = nullptr;
        DebugManager * debugManager = nullptr;
        Device * device = nullptr;
        
        vk::Instance instance;

        void createVulkanInstance(const std::string& applicationName, const unsigned int& applicationVersion, const std::string& engineName, const unsigned int& engineVersion);

        bool enumerateInstanceExtensions() const;
        std::vector<const char *> getRequiredExtensions() const;
};

}

#endif /* pecs_hpp */
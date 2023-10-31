/*
*   PECS - pecs.hpp
*   Author:     Matthew Hipp
*   Created:    6/29/23
*   Updated:    10/29/23
*/

#ifndef pecs_hpp
#define pecs_hpp

#define VULKAN_HPP_NO_CONSTRUCTORS
#define GLFW_INCLUDE_NONE

#include <algorithm>
#include <iostream>
#include <limits>
#include <optional>
#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_to_string.hpp>
#include <GLFW/glfw3.h>

namespace pecs
{

class Engine;
class Window;
class Device;

enum QueueType
{
    Graphics,
    Present,
    Compute
};

struct InitializationInfo
{   
    std::string applicationName = "PECS Application";
    unsigned int applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

    std::string windowTitle = "PECS Application";
    unsigned int windowWidth = 600, windowHeight = 600;
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

struct SwapchainImageDetails
{
    vk::Extent2D extent;
    vk::Format format;
};

class Engine
{
    public:
        class Main
        {
            public:
                virtual void operator()() = 0;
        };
    
    public:
        Engine() = default;
        Engine(const InitializationInfo * initInfo);
        Engine(const Engine&) = delete;
        Engine(Engine&&) = delete;
        
        ~Engine();

        Engine& operator=(const Engine&) = delete;
        Engine& operator=(Engine&&) = delete;

        void initialize(const InitializationInfo * initInfo);
        void run();
        void run(Main& mainLoop);

    private:
        Window * window = nullptr;
        Device * device = nullptr;
        
        std::vector<const char *> layers;
        
        SwapchainImageDetails swapchainImageDetails;
        std::vector<vk::Image> swapchainImages;
        
        #ifdef NDEBUG
            const bool enableValidationLayers = false;
        #else
            const bool enableValidationLayers = true;
        #endif

        vk::Instance instance = VK_NULL_HANDLE;
        vk::SwapchainKHR swapchain = VK_NULL_HANDLE;
        std::vector<vk::ImageView> swapchainImageViews;

        void createVulkanInstance(const std::string& applicationName, const unsigned int& applicationVersion);
        void createSwapchain();
        void createImageViews();

        bool enumerateInstanceExtensions() const;
        std::vector<const char *> getRequiredExtensions() const;
        vk::SurfaceFormatKHR chooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats) const;
        vk::PresentModeKHR chooseSwapchainPresentMode(const std::vector<vk::PresentModeKHR>& availableModes) const;
        vk::Extent2D chooseSwapchainExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const GLFWwindow * window) const;
};

class Window
{
    public:
        Window(unsigned int width, unsigned int height, std::string title);
        Window(const Window&) = delete;
        
        ~Window();

        Window& operator=(const Window&) = delete;
        
        const GLFWwindow * getGLFWwindow() const;
        const vk::SurfaceKHR& getSurface() const;
        
        bool shouldClose() const;

        void createSurface(const vk::Instance& instance);
        void destroySurface(const vk::Instance& instance);

    private:
        GLFWwindow * window = nullptr;
        vk::SurfaceKHR surface;
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
        const std::vector<const char *> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        
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

#endif /* pecs_hpp */
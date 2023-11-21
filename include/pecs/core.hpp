/*
*   PECS - core.hpp
*   Author:     Matthew Hipp
*   Created:    6/29/23
*   Updated:    11/15/23
*/

#ifndef pecs_core_hpp
#define pecs_core_hpp

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

#include "enums.hpp"
#include "structs.hpp"
#include "objects.hpp"

namespace pecs
{

class Engine;
class Window;
class Device;

class Engine
{
    public:
        class Main
        {
            public:
                Main(const Engine& engine) : device(engine.getDevice()), details(engine.swapchainImageDetails) {}
                
                ~Main()
                {
                    for (int i = 0; i < objects.size(); ++i)
                        objects[i].destroy(device);
                }

                virtual void operator()() = 0;

                void loadObjects()
                {        
                    for (int i = 0; i < objects.size(); ++i)
                    {
                        std::cout << "\tloading object " << i + 1 << "/" << objects.size() << "...\n";
                        if (0x10 & objects[i].pipelineTypes)
                            objects[i].graphicsPipeline = new GraphicsPipeline(device, details, objects[i].shaderPaths);

                        if (objects[i].graphicsPipeline != nullptr)
                            std::cout << "\t\tcreated graphics pipeline for object " << i << '\n';
                    }            
                }

            public:
                std::vector<Object> objects;

            private:
                const Device * device;
                const SwapchainImageDetails details;
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
        void run(Main&);

        const Device * getDevice() const;

    public:
        SwapchainImageDetails swapchainImageDetails;

    private:
        Window * window = nullptr;
        Device * device = nullptr;
        
        std::vector<const char *> layers;
        
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
        
        bool shouldClose() const;

        void createSurface(const vk::Instance& instance);
        void destroySurface(const vk::Instance& instance);

    public:
        vk::SurfaceKHR surface;
    
    private:
        GLFWwindow * window = nullptr;
};

class Device
{
    public:
        Device(std::vector<vk::PhysicalDevice> devices, const vk::SurfaceKHR& surface);
        Device(const Device&) = delete;
        Device(Device&&) = delete;

        ~Device();

        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&) = delete;

        SwapchainSupportDetails querySwapchainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) const;
        QueueFamilyIndices findPhysicalDeviceQueueFamilyIndicies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) const;

    private:
        void choosePhysicalDevice(std::vector<vk::PhysicalDevice> devices, const vk::SurfaceKHR& surface);
        void createLogicalDevice(const vk::SurfaceKHR& surface);
        
        std::vector<vk::PhysicalDevice> getSuitablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices, const vk::SurfaceKHR& surface) const;
        unsigned int evaluate(const vk::PhysicalDeviceType type) const;
        bool checkPhysicalDeviceExtensionSupport(const vk::PhysicalDevice& device) const;
    
    public:
        vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
        vk::Device logicalDevice = VK_NULL_HANDLE;

        vk::Queue graphicsQueue = VK_NULL_HANDLE;
        vk::Queue presentQueue = VK_NULL_HANDLE;
        vk::Queue computeQueue = VK_NULL_HANDLE;
    
    private:
         // required device extensions
        const std::vector<const char *> deviceExtensions = { 
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
        };  
};

}

#endif /* pecs_core_hpp */
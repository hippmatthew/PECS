/*
*   PECS - pecs.hpp
*   Author:     Matthew Hipp
*   Created:    6/29/23
*   Updated:    7/22/23
*/

#ifndef pecs_hpp
#define pecs_hpp

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cstring>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

namespace pecs
{

struct InitializationInfo
{
    std::string applicationName = "PECS Application";
    unsigned int applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

    std::string windowTitle = "PECS Application";
    unsigned int windowWidth = 600, windowHeight = 600;

    bool enableDebugManager = true;
};

struct EngineInfo
{
    std::string name;
    unsigned int version;
};

struct QueueFamilyIndices
{
    std::optional<unsigned int> graphics;
    std::optional<unsigned int> compute;

    bool isComplete() { return graphics.has_value() && compute.has_value(); }
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

        std::string vkResultToString(const vk::Result result) const;
};

class Device
{
    public:
        Device(const vk::Instance& instance, const DebugManager * dm);
        Device(const Device&) = delete;

        ~Device() = default;

        Device& operator=(const Device&) = delete;

        vk::PhysicalDeviceProperties getPhysicalDeviceProperties() const;

    private:
        const DebugManager * debugManager;
        
        vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
        vk::Device logicalDevice = VK_NULL_HANDLE;

        void choosePhysicalDevice(const vk::Instance& instance);
        std::vector<vk::PhysicalDevice> getSuitablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices) const;
        QueueFamilyIndices findPhysicalDeviceQueueFamilyIndicies(const vk::PhysicalDevice& device, const  std::vector<vk::QueueFamilyProperties>& queueFamilies) const;
        unsigned int evaluate(vk::PhysicalDeviceType type) const;
        std::string vkPhysicalDeviceTypeToString(const vk::PhysicalDeviceType type) const;
};

class Window
{
    public:
        Window(unsigned int width, unsigned int height, std::string title, const DebugManager * dm);
        Window(const Window&) = delete;
        
        ~Window();

        Window& operator=(const Window&) = delete;
        
        bool shouldClose() const;

    private:
        GLFWwindow * window = nullptr;
        const DebugManager * debugManager;
};;

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
        const EngineInfo engineInfo{ .name = "PECS",
                                     .version = VK_MAKE_API_VERSION(0, 1, 0, 0) };
        
        Window * window = nullptr;
        DebugManager * debugManager = nullptr;
        Device * device = nullptr;
        
        vk::Instance instance;

        void createVulkanInstance(std::string applicationName, unsigned int applicationVersion);

        bool enumerateInstanceExtensions() const;
        std::vector<const char *> getRequiredExtensions() const;
};

}

#endif /* pecs_hpp */
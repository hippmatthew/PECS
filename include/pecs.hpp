/*
*   PECS - pecs.hpp
*   Author:     Matthew Hipp
*   Created:    6/29/23
*   Updated:    6/29/23
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
};

struct EngineInfo
{
    std::string name;
    unsigned int version;
};

class DebugManager
{
    public:
        DebugManager(const DebugManager&) = delete;
        ~DebugManager();

        DebugManager& operator=(const DebugManager&) = delete;
        
        std::vector<const char *> getValidationLayers() const;
        
        void setupDebugMessenger(const vk::Instance& instance) const;
        bool checkValidationLayerSupport() const;
        
        void deallocate(const vk::Instance& instance) const;
        
        static bool isEnabled();
        static void initialize();
        static DebugManager* getInstance();
        static void populateMessengerStruct(vk::DebugUtilsMessengerCreateInfoEXT& info);
        
    private:
        DebugManager();
        static DebugManager * sInstance;

        vk::DebugUtilsMessengerEXT debugMessenger;
        
        #ifdef NDEBUG
            static const bool debugMode = false;
        #else
            static const bool debugMode = true;
        #endif

        std::vector<const char *> validationLayers = { "VK_LAYER_KHRONOS_validation" };

        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                              VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                              void * pUserData);
};

class Window
{
    public:
        Window(unsigned int width, unsigned int height, std::string title);
        Window(const Window&) = delete;
        ~Window();

        Window& operator=(const Window&) = delete;

        bool shouldClose() const;

    private:
        GLFWwindow * window;
};

class Engine
{
    public:
        Engine() = default;
        Engine(const Engine&) = delete;
        ~Engine();

        Engine& operator=(const Engine&) = delete;

        bool isActive() const;
        Window* getWindow() const;
        void getEvents() const;

        void initialize(const InitializationInfo* initInfo);

    private:
        const EngineInfo engineInfo{ .name = "PECS",
                                     .version = VK_MAKE_API_VERSION(0, 1, 0, 0) };
        
        DebugManager * debugManager;
        Window * window;
        vk::Instance instance;

        void createVulkanInstance(std::string applicationName, unsigned int applicationVersion);

        bool enumerateInstanceExtensions() const;
        std::vector<const char *> getRequiredExtensions() const;
};

}

#endif /* pecs_hpp */
/*
*   PECS - pecs.hpp
*   Author:     Matthew Hipp
*   Created:    6/29/23
*   Updated:    6/29/23
*/

#ifndef pecs_hpp
#define pecs_hpp

#include <string>

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

class Window
{
    public:
        Window(unsigned int width, unsigned int height, std::string title);
        Window(const Window&) = delete;
        ~Window();

        Window& operator=(const Window&) = delete;

        bool shouldClose() const;
        void pollEvents() const;

    private:
        GLFWwindow * window;
};

class Engine
{
    public:
        Engine();
        Engine(const Engine&) = delete;
        ~Engine();

        Engine& operator=(const Engine&) = delete;

        bool isActive() const;
        Window* getWindow() const;

        void initialize(const InitializationInfo* initInfo);

    private:
        const EngineInfo engineInfo{ .name = "PECS",
                                     .version = VK_MAKE_API_VERSION(0, 1, 0, 0) };
        
        Window * window;
        vk::Instance instance;

        void createVulkanInstance(std::string applicationName, unsigned int applicationVersion);
        
        bool enumerateInstanceExtensions() const;
    };

}

#endif /* pecs_hpp */
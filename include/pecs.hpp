/*
*   PECS - pecs.hpp
*   Author:     Matthew Hipp
*   Created:    6/29/23
*   Updated:    6/29/23
*/

#ifndef pecs_hpp
#define pecs_hpp

#include <string>

#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace pecs
{

struct InitializationInfo
{
    std::string applicationName;
    unsigned int applicationVersion;

    std::string windowTitle;
    unsigned int windowWidth, windowHeight;
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

        bool ShouldClose() const;
        void PollEvents() const;

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

        bool IsActive() const;
        Window* GetWindow() const;

        void Initialize(const InitializationInfo* initInfo);

    private:
        const EngineInfo engineInfo{ .name = "PECS",
                                     .version = VK_MAKE_API_VERSION(0, 1, 0, 0) };
        
        Window * window;
};

}

#endif /* pecs_hpp */
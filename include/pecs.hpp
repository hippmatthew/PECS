/*
*   PECS - pecs.hpp
*   Author:     Matthew Hipp
*   Created:    6/29/23
*   Updated:    6/29/23
*/

#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace pecs
{

struct ApplicationInfo
{
    const char * applicationName;
    uint32_t applicationVersion;

    unsigned int windowWidth, windowHeight;
    const char * windowTitle;
};

struct EngineInfo
{
    const char * name;
    uint32_t version;
};

class Window
{
    public:
        Window(unsigned int width, unsigned int height, const char * title);
        Window(const Window&) = delete;
        ~Window();

        Window& operator=(const Window&) = delete;

        GLFWwindow* GetWindow() const;

    private:
        GLFWwindow * window;
};

class Engine
{
    public:
        Engine(const ApplicationInfo* appInfo);
        Engine(const Engine&) = delete;
        ~Engine();

        Engine& operator=(const Engine&) = delete;

    private:
        const EngineInfo engineInfo{ .name = "PECS",
                                     .version = VK_MAKE_API_VERSION(0, 1, 0, 0) };
        
        Window * window;
};



}
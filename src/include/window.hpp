/*
*   PECS - window.hpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    7/25/23
*/

#ifndef pecs_window_hpp
#define pecs_window_hpp

#ifndef VULKAN_HPP_NO_CONSTRUCTORS
    #define VULKAN_HPP_NO_CONSTRUCTORS
#endif /* VULKAN_HPP_NO_CONSTRUCTORS */

#include <vulkan/vulkan.hpp>

#ifndef GLFW_INCLUDE_NONE
    #define GLFW_INCLUDE_NONE
#endif /* GLFW_INCLUDE_NONE */

#include <GLFW/glfw3.h>

#include "debug.hpp"

namespace pecs
{

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

}

#endif /* pecs_window_hpp */
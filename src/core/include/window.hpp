/*
*   PECS - window.hpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    10/29/23
*/

#ifndef pecs_window_hpp
#define pecs_window_hpp

#define VULKAN_HPP_NO_CONSTRUCTORS
#define GLFW_INCLUDE_NONE

#include <iostream>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_to_string.hpp>
#include <GLFW/glfw3.h>

namespace pecs
{

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

}

#endif /* pecs_window_hpp */
/*
 *  PECS - window.cpp
 *  Author:     Matthew Hipp
 *  Created:    6/27/23
 *  Updated:    10/29/23
 */

#include "include/window.hpp"

namespace pecs
{

Window::Window(unsigned int width, unsigned int height, std::string title)
{   
    std::cout << "\tinitalizing glfw...\n";
    glfwInit();

    std::cout << "\tsetting window hints...\n";
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    std::cout << "\tcreating glfw window...\n";
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    std::cout << "\twindow created\n";
}

Window::~Window()
{    
    std::cout << "\tdestroying glfw window...\n";
    glfwDestroyWindow(window);

    std::cout << "\tterminating glfw...\n";
    glfwTerminate();
}

const GLFWwindow * Window::getGLFWwindow() const
{ return window; }

bool Window::shouldClose() const 
{ return glfwWindowShouldClose(window); }

void Window::createSurface(const vk::Instance& instance)
{
    vk::Result result = static_cast<vk::Result>(glfwCreateWindowSurface(instance, window, nullptr, reinterpret_cast<VkSurfaceKHR *>(&surface)));
    switch (result)
    {
        case vk::Result::eSuccess:
            std::cout << "\tglfw window surface created\n";
            break;
        default:
            throw std::runtime_error(vk::to_string(result));
    }
}

void Window::destroySurface(const vk::Instance& instance)
{ instance.destroySurfaceKHR(surface); }

}
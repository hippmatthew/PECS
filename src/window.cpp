/*
*   PECS - window.cpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    6/29/23
*/

#include "include/window.hpp"

namespace pecs
{

Window::Window(unsigned int width, unsigned int height, const char * title)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* Window::GetWindow() const
{ return window; }

}
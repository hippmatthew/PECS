/*
 *  PECS - window.cpp
 *  Author:     Matthew Hipp
 *  Created:    6/27/23
 *  Updated:    7/21/23
 */

#include "include/window.hpp"

namespace pecs
{

Window::Window(unsigned int width, unsigned int height, std::string title, const DebugManager * dm) : debugManager(dm)
{   
    if (debugManager->isEnabled()) debugManager->message("\tinitalizing glfw...");
    glfwInit();

    if (debugManager->isEnabled()) debugManager->message("\tsetting window hints...");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if (debugManager->isEnabled()) debugManager->message("\tcreating glfw window...");
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (debugManager->isEnabled()) debugManager->message("\twindow created");
}

Window::~Window()
{    
    if (debugManager->isEnabled()) debugManager->message("\tdestroying glfw window...");
    glfwDestroyWindow(window);

    if (debugManager->isEnabled()) debugManager->message("\tterminating glfw...");
    glfwTerminate();
}

bool Window::shouldClose() const
{ return glfwWindowShouldClose(window); }

}
/*
*   PECS - window.hpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    6/27/23
*/

#ifndef pecs_window_hpp
#define pecs_window_hpp

#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "debug.hpp"

namespace pecs
{

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
        DebugManager * debugManager = DebugManager::getInstance();
        
        GLFWwindow * window;
};

}

#endif /* pecs_window_hpp */
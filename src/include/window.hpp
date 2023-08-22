/*
*   PECS - window.hpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    7/21/23
*/

#ifndef pecs_window_hpp
#define pecs_window_hpp

#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <memory>

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
        
        bool shouldClose() const;

    private:
        GLFWwindow * window = nullptr;
        const DebugManager * debugManager;
};

}

#endif /* pecs_window_hpp */
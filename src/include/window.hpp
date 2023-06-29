/*
*   PECS - window.hpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    6/27/23
*/

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace pecs
{

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

}
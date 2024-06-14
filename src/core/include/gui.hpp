#ifndef vecs_core_gui_hpp
#define vecs_core_gui_hpp

#include "src/core/include/settings.hpp"

#ifndef vecs_include_glfw
#define vecs_include_glfw

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#endif // vecs_include_glfw

namespace vecs
{

class GUI
{
  public:
    GUI();
    GUI(const GUI&) = delete;
    GUI(GUI&&) = delete;

    ~GUI();

    GUI& operator = (const GUI&) = delete;
    GUI& operator = (GUI&&) = delete;

    std::vector<const char *> extensions() const;
    bool shouldClose() const;
    void pollEvents() const;

  private:
    GLFWwindow * gl_window = nullptr;
};

} // namespace vecs

#endif // vecs_core_settings_hpp
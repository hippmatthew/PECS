#include "src/core/include/gui.hpp"

namespace vecs
{

GUI::GUI()
{
  Settings settings = Settings::instance();
  
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  gl_window = glfwCreateWindow(settings.width(), settings.height(), settings.title().c_str(), nullptr, nullptr);
};

GUI::~GUI()
{
  glfwDestroyWindow(gl_window);
  glfwTerminate();
}

std::vector<const char *> GUI::extensions() const
{
  unsigned int extensionCount = 0;
  const char ** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

  return std::vector<const char *>(extensions, extensions + extensionCount);
}

bool GUI::shouldClose() const
{
  return glfwWindowShouldClose(gl_window);
}

void GUI::pollEvents() const
{
  glfwPollEvents();
}

const vk::raii::SurfaceKHR& GUI::surface() const
{
  return vk_surface;
}

void GUI::createSurface(const vk::raii::Instance& vk_instance)
{
  vk::raii::SurfaceKHR::CType surface;
  glfwCreateWindowSurface(*vk_instance, gl_window, nullptr, &surface);
  
  vk_surface = vk::raii::SurfaceKHR(vk_instance, surface);
}

} // namespace vecs
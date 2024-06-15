#ifndef vecs_core_gui_hpp
#define vecs_core_gui_hpp

#include "src/core/include/device.hpp"
#include "src/core/include/settings.hpp"

#ifndef vecs_include_glfw
#define vecs_include_glfw

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#endif // vecs_include_glfw

namespace vecs
{

class Device;

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
    const vk::raii::SurfaceKHR& surface() const;

    void createSurface(const vk::raii::Instance&);
    void setupWindow(const vecs::Device&);
  
  private:
    void chooseSurfaceFormat(const vk::raii::PhysicalDevice&) const;
    void choosePresentMode(const vk::raii::PhysicalDevice&) const;
    void chooseExtent(const vk::raii::PhysicalDevice&) const;

    void createSwapchain(const vk::raii::PhysicalDevice&, const vk::raii::Device&);
    void createImageViews(const vk::raii::Device&);

  private:
    GLFWwindow * gl_window = nullptr;

    vk::raii::SurfaceKHR vk_surface = nullptr;
    
    vk::raii::SwapchainKHR vk_swapchain = nullptr;
    std::vector<vk::Image> vk_images;
    std::vector<vk::raii::ImageView> vk_imageViews;
};

} // namespace vecs

#endif // vecs_core_settings_hpp
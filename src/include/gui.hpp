/*
 *  PECS - gui.hpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  1/21/24
 */

#ifndef pecs_gui_hpp
#define pecs_gui_hpp

#include "src/include/singular.hpp"
#include "src/include/settings.hpp"

#ifndef pecs_include_glfw
#define pecs_include_glfw

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#endif // pecs_include_glfw

#include <vector>

namespace pecs
{

class GUI : public Singular
{
  public:
    GUI();
    GUI(const Settings::GUI&);

    ~GUI();

    std::vector<const char *> extensions() const;
    GLFWwindow * window() const;
    const vk::raii::SurfaceKHR& surface() const;

    void createSurface(const vk::raii::Instance&);
    void setupWindow(const vk::raii::PhysicalDevice&, const vk::raii::Device&);

  private:
    void initialize();
    void createSwapchain(const vk::raii::PhysicalDevice&, const vk::raii::Device&);
    void chooseSurfaceFormat(const vk::raii::PhysicalDevice&);
    void choosePresentMode(const vk::raii::PhysicalDevice&);
    void chooseExtent(const vk::raii::PhysicalDevice&);
    void createImageViews(const vk::raii::Device&);
  
  private:
    Settings::GUI settings;

    GLFWwindow * gl_window;

    vk::raii::SurfaceKHR vk_surface = nullptr;
    vk::raii::SwapchainKHR vk_swapchain = nullptr;
    std::vector<vk::Image> vk_images;
    std::vector<vk::raii::ImageView> vk_imageViews;
    vk::SurfaceFormatKHR vk_surfaceFormat;
    vk::PresentModeKHR vk_presentMode;
    vk::Extent2D vk_extent;
};

} // namespace pecs

#endif // pecs_gui_hpp
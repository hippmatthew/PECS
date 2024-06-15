#ifndef vecs_core_settings_hpp
#define vecs_core_settings_hpp

#ifndef vecs_include_vulkan
#define vecs_include_vulkan

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#endif // vecs_include_vulkan

#include <string>

namespace vecs
{

class Settings
{ 
  public:
    Settings(const Settings&) = delete;
    Settings(Settings&&) = delete;

    ~Settings();
    
    Settings& operator = (const Settings&) = delete;
    Settings& operator = (Settings&&) = delete;
    
    static Settings& instance();
    static void destroy();

    std::string name() const;
    unsigned int version() const;
    bool validation_enabled() const;
    std::string title() const;
    unsigned int width() const;
    unsigned int height() const;
    bool portability_enabled() const;
    std::vector<const char *> device_extensions() const;
    vk::Format format() const;
    vk::ColorSpaceKHR color_space() const;
    vk::PresentModeKHR present_mode() const;
    vk::Extent2D extent() const;

    Settings& update_name(std::string);
    Settings& update_version(unsigned int);
    Settings& toggle_validation();
    Settings& update_title(std::string);
    Settings& update_width(unsigned int);
    Settings& update_height(unsigned int);
    Settings& toggle_portability();
    Settings& add_device_extension(const char *);
    Settings& remove_device_extension(const char *);
    Settings& update_format(vk::Format);
    Settings& update_color_space(vk::ColorSpaceKHR);
    Settings& update_present_mode(vk::PresentModeKHR);
    Settings& update_extent(unsigned int width, unsigned int height);
    
    void set_default();

  private:
    Settings() = default;
  
  private:
    static Settings * p_settings;
    
    std::string s_name = "VECS Application";
    unsigned int s_version = VK_MAKE_API_VERSION(0, 1, 0, 0);
    bool s_validationEnabled = true;
    
    std::string s_title = "VECS Application";
    unsigned int s_width = 1280;
    unsigned int s_height = 720;

    bool s_portabilityEnabled = false;

    std::vector<const char *> s_gpuExtensions{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };

    vk::Format s_format = vk::Format::eB8G8R8A8Srgb;
    vk::ColorSpaceKHR s_colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    vk::PresentModeKHR s_presentMode = vk::PresentModeKHR::eMailbox;
    vk::Extent2D s_extent{
      .width = s_width,
      .height = s_height
    };
};

} // namespace vecs

#endif // vecs_core_settings_hpp
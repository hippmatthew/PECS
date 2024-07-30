#ifndef vecs_core_settings_hpp
#define vecs_core_settings_hpp

#ifndef vecs_include_vulkan
#define vecs_include_vulkan

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#endif // vecs_include_vulkan

#include <map>
#include <numeric>
#include <string>

#define VECS_LIMIT      std::numeric_limits<unsigned short>::max()
#define VECS_SETTINGS   vecs::Settings::instance()

namespace vecs
{

class Settings
{
  public:
    Settings(const Settings&) = delete;
    Settings(Settings&&) = delete;

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
    float scale_x() const;
    float scale_y() const;
    float aspect_ratio() const;
    bool portability_enabled() const;
    std::vector<const char *> device_extensions() const;
    vk::Format format() const;
    vk::ColorSpaceKHR color_space() const;
    vk::PresentModeKHR present_mode() const;
    vk::Extent2D extent() const;
    vk::Format depth_format() const;
    unsigned long max_flight_frames() const;
    vk::ClearValue background_color() const;
    const unsigned short& max_entities() const;
    const unsigned short& max_components() const;

    template <typename T>
    unsigned short component_id();

    Settings& update_name(std::string);
    Settings& update_version(unsigned int);
    Settings& toggle_validation();
    Settings& update_title(std::string);
    Settings& update_width(unsigned int);
    Settings& update_height(unsigned int);
    Settings& update_scale(float, float);
    Settings& toggle_portability();
    Settings& add_device_extension(const char *);
    Settings& remove_device_extension(const char *);
    Settings& update_format(vk::Format);
    Settings& update_color_space(vk::ColorSpaceKHR);
    Settings& update_present_mode(vk::PresentModeKHR);
    Settings& update_extent(unsigned int width, unsigned int height);
    Settings& update_max_flight_frames(unsigned long);
    Settings& update_background_color(vk::ClearValue);
    Settings& update_max_entities(unsigned short);
    Settings& update_max_components(unsigned short);

    void set_default();

  private:
    Settings() = default;
    ~Settings() = default;

  private:
    static Settings * p_settings;

    std::map<const char *, unsigned long> s_idMap;
    unsigned int nextID = 0;

    std::string s_name = "VECS Application";
    unsigned int s_version = VK_MAKE_API_VERSION(0, 1, 0, 0);
    bool s_validationEnabled = true;

    std::string s_title = "VECS Application";
    unsigned int s_width = 1280;
    unsigned int s_height = 720;
    float s_scalex = 1.0f;;
    float s_scaley = 1.0f;

    bool s_portabilityEnabled = false;

    std::vector<const char *> s_gpuExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    vk::Format s_format = vk::Format::eB8G8R8A8Srgb;
    vk::ColorSpaceKHR s_colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    vk::PresentModeKHR s_presentMode = vk::PresentModeKHR::eMailbox;
    vk::Extent2D s_extent{
      .width = s_width,
      .height = s_height
    };

    vk::Format s_dFormat = vk::Format::eD32Sfloat;

    unsigned long s_maxFrames = 2;
    vk::ClearValue s_backColor = vk::ClearValue{
      vk::ClearColorValue{std::array<float, 4>{ 0.0025f, 0.01f, 0.005f, 1.0f }}
    };

    unsigned short s_maxEntities = 20000;
    unsigned short s_maxComponents = 100;
};

} // namespace vecs

#include "src/core/include/settings_templates.hpp"

#endif // vecs_core_settings_hpp
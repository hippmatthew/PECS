#include "src/core/include/settings.hpp"

namespace vecs
{

Settings * Settings::p_settings = nullptr;

Settings& Settings::instance()
{
  if (p_settings == nullptr)
    p_settings = new Settings;
    
  return *p_settings;
}

void Settings::destroy()
{
  if (p_settings == nullptr) return;
  
  delete p_settings;
  p_settings = nullptr;
}

std::string Settings::name() const
{
  return s_name;
}

unsigned int Settings::version() const
{
  return s_version;
}

bool Settings::validation_enabled() const
{
  return s_validationEnabled;
}

std::string Settings::title() const
{
  return s_title;
}

unsigned int Settings::width() const
{
  return s_width;
}

unsigned int Settings::height() const
{
  return s_height;
}

bool Settings::portability_enabled() const
{
  return s_portabilityEnabled;
}

std::vector<const char *> Settings::device_extensions() const
{
  return s_gpuExtensions; 
}

vk::Format Settings::format() const
{
  return s_format;
}

vk::ColorSpaceKHR Settings::color_space() const
{
  return s_colorSpace;
}

vk::PresentModeKHR Settings::present_mode() const
{
  return s_presentMode;
}

vk::Extent2D Settings::extent() const
{
  return s_extent;
}

unsigned long Settings::max_flight_frames() const
{
  return s_maxFrames;
}

vk::ClearValue Settings::background_color() const
{
  return s_backColor;
}

Settings& Settings::update_name(std::string newName)
{
  s_name = newName;
  return *this;
}

Settings& Settings::update_version(unsigned int newVersion)
{
  s_version = newVersion;
  return *this;
}

Settings& Settings::toggle_validation()
{
  s_validationEnabled = !s_validationEnabled;
  return *this;
}

Settings& Settings::update_title(std::string newTitle)
{
  s_title = newTitle;
  return *this;
}

Settings& Settings::update_width(unsigned int newWidth)
{
  s_width = newWidth;
  return *this;
}

Settings& Settings::update_height(unsigned int newHeight)
{
  s_height = newHeight;
  return *this;
}

Settings& Settings::toggle_portability()
{
  s_portabilityEnabled = !s_portabilityEnabled;
  return *this;
}

Settings& Settings::add_device_extension(const char * ext)
{
  s_gpuExtensions.emplace_back(ext);
  return *this;
}

Settings& Settings::remove_device_extension(const char * ext)
{
  unsigned long index = 0;
  for (const auto * extension : s_gpuExtensions)
  {
    if (std::string(extension) == std::string(ext))
    {
      s_gpuExtensions.erase(s_gpuExtensions.begin() + index);
      break;
    }
    ++index;
  }

  return *this;
}

Settings& Settings::update_format(vk::Format f)
{
  s_format = f;
  return *this;
}

Settings& Settings::update_color_space(vk::ColorSpaceKHR cs)
{
  s_colorSpace = cs;
  return *this;
}

Settings& Settings::update_present_mode(vk::PresentModeKHR pm)
{
  s_presentMode = pm;
  return *this;
}

Settings& Settings::update_extent(unsigned int w, unsigned int h)
{
  s_extent = vk::Extent2D{
    .width = w,
    .height = h
  };
  return *this;
}

Settings& Settings::update_max_flight_frames(unsigned long frames)
{
  s_maxFrames = frames;
  return *this;
}

Settings& Settings::update_background_color(vk::ClearValue color)
{
  s_backColor = color;
  return *this;
}

void Settings::set_default()
{
  s_name = s_title = "VECS Application";
  s_version = VK_MAKE_API_VERSION(0, 1, 0, 0);
  s_validationEnabled = true;
  s_width = 1280;
  s_height = 720;
  s_portabilityEnabled = false;
  s_gpuExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
  };
  s_format = vk::Format::eB8G8R8A8Srgb;
  s_colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
  s_presentMode = vk::PresentModeKHR::eMailbox;
  s_extent = vk::Extent2D{
    .width = s_width,
    .height = s_height
  };
  s_maxFrames = 2;
  s_backColor = vk::ClearValue{vk::ClearColorValue{std::array<float, 4>{ 0.0025f, 0.01f, 0.005f, 1.0f }}};
}

} // namespace vecs
#include "src/core/include/settings.hpp"

namespace vecs
{

Settings * Settings::p_settings = nullptr;

Settings::~Settings()
{
  destroy();
}

Settings& Settings::instance()
{
  if (p_settings == nullptr)
    p_settings = new Settings;
  return *p_settings;
}

void Settings::destroy()
{
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

void Settings::set_default()
{
  s_name = s_title = "VECS Application";
  s_version = VK_MAKE_API_VERSION(0, 1, 0, 0);
  s_validationEnabled = true;
  s_width = 1280;
  s_height = 720;
}

} // namespace vecs
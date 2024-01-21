/*
 *  PECS - settings.hpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  1/21/24
 */

#ifndef pecs_settings_hpp
#define pecs_settings_hpp

#ifndef pecs_include_vulkan
#define pecs_include_vulkan

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_to_string.hpp>

#endif // pecs_include_vulkan

#include <string>

#define PECS_VALIDATION_BIT 0x1u

namespace pecs
{

class Settings
{
  public:
    struct Engine
    {
      std::string applicationName = "PECS Application";
      unsigned int applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
      unsigned int layerBits = PECS_VALIDATION_BIT;
    };

    struct GUI
    {
      std::string windowTitle = "PECS Application";
      int width = 1280;
      int height = 720;
    };

  public:
    Settings() = default;
    Settings(const Settings&);
    Settings(Settings&&) = delete;

    ~Settings() = default;

    Settings& operator = (const Settings&);
    Settings& operator = (Settings&&) = delete;

  public:
    Settings::Engine engine;
    Settings::GUI gui;
};

} // namespace pecs

#endif // pecs_settings_hpp
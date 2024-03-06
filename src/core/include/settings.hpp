/*
 *  PECS::core - settings.hpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  3/5/24
 */

#ifndef pecs_core_settings_hpp
#define pecs_core_settings_hpp

#ifndef pecs_include_vulkan
#define pecs_include_vulkan

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

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
      float fov = 90.0f;
    };

    struct GUI
    {
      std::string windowTitle = "PECS Application";
      int width = 1280;
      int height = 720;
    };

    struct Renderer
    {
      unsigned int maxFlightFrames = 2;
      vk::ClearValue backgroundColor = vk::ClearValue{ vk::ClearColorValue{ std::array<float, 4>{ 0.0025f, 0.01f, 0.005f, 1.0f } } };
    };

  public:
    Settings() = default;
    Settings(const Settings&) = default;
    Settings(Settings&&) = default;

    ~Settings() = default;

    Settings& operator = (const Settings&) = default;
    Settings& operator = (Settings&&) = default;

  public:
    Settings::Engine engine;
    Settings::GUI gui;
    Settings::Renderer renderer;
};

} // namespace pecs

#endif // pecs_core_settings_hpp
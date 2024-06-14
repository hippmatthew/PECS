#include "src/core/include/engine.hpp"

#define VK_VALIDATION_LAYER_NAME "VK_LAYER_KHRONOS_validation"
#define VECS_ENGINE_VERSION       VK_MAKE_API_VERSION(0, 1, 2, 6)

namespace vecs
{

Engine::~Engine()
{
  delete device;
  delete gui;
}

void Engine::initialize()
{
  gui = new GUI;
  createInstance();
  gui->createSurface(vk_instance);
  device = new Device(vk_instance, *gui);
}

void Engine::run()
{
  while (!gui->shouldClose())
  {
    gui->pollEvents();
  }
}

void Engine::createInstance()
{
  vk::raii::Context vk_context;

  std::string name = Settings::instance().name();
  vk::ApplicationInfo i_application{
    .pApplicationName   = name.c_str(),
    .applicationVersion = Settings::instance().version(),
    .pEngineName        = "VECS",
    .engineVersion      = VECS_ENGINE_VERSION,
    .apiVersion         = VK_API_VERSION_1_3
  };

  std::vector<const char *> layers;
  if (Settings::instance().validation_enabled()) layers.emplace_back(VK_VALIDATION_LAYER_NAME);
  
  auto extensions = gui->extensions();
  for (const auto& extension : vk_context.enumerateInstanceExtensionProperties())
  {
    if (std::string(extension.extensionName) == VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)
    {
      extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
      
      Settings::instance().toggle_portability();
      break;
    }
  }
  extensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
  
  vk::InstanceCreateInfo ci_instance{
    .pApplicationInfo         = &i_application,
    .enabledLayerCount        = static_cast<unsigned int>(layers.size()),
    .ppEnabledLayerNames      = layers.data(),
    .enabledExtensionCount    = static_cast<unsigned int>(extensions.size()),
    .ppEnabledExtensionNames  = extensions.data()
  };
  if (Settings::instance().portability_enabled()) ci_instance.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;

  vk_instance = vk_context.createInstance(ci_instance);
}

} // namespace vecs
/*
 *  PECS - engine.cpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  1/26/24
 */

#include "src/include/engine.hpp"

#define PECS_ENGINE_NAME "PECS"
#define PECS_ENGINE_VERSION VK_MAKE_API_VERSION(0, 1, 0, 0)

namespace pecs
{

Engine::Engine()
{
  gui = new GUI;
  initialize();
}

Engine::Engine(const Settings& s)
{
  settings = s.engine;
  gui = new GUI(s.gui);

  initialize();
}

Engine::~Engine()
{
  delete gui;
  delete device;
}

const Device& Engine::getDevice() const
{
  return *device;
}

const ViewportInfo Engine::viewportInfo() const
{
  ViewportInfo i_viewport;
  i_viewport.first = gui->extent();
  i_viewport.second = gui->format();

  return i_viewport; 
}

void Engine::run(Loop&)
{
  unsigned int loopCount = 0;
  while (!glfwWindowShouldClose(gui->window()))
  {
    glfwPollEvents();

    Loop();
  }
}

void Engine::initialize()
{
  if (settings.layerBits & PECS_VALIDATION_BIT)
    layers.emplace_back("VK_LAYER_KHRONOS_validation");

  createInstance();

  gui->createSurface(vk_instance);

  device = new Device(vk_instance, gui->surface());

  gui->setupWindow(device->physical(), device->logical());
}

void Engine::createInstance()
{
  vk::raii::Context vk_context;
  
  vk::ApplicationInfo i_application{
    .pApplicationName   = settings.applicationName.c_str(),
    .applicationVersion = settings.applicationVersion,
    .pEngineName        = PECS_ENGINE_NAME,
    .engineVersion      = PECS_ENGINE_VERSION,
    .apiVersion         = VK_API_VERSION_1_3
  };

  auto extensions = gui->extensions();

  bool hasPortabilityBit = false;
  for (const auto& extension : vk_context.enumerateInstanceExtensionProperties())
  {
    if (std::string(extension.extensionName) == VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)
    {
      extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
      extensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
      
      hasPortabilityBit = true;
      break;
    }
  }

  vk::InstanceCreateInfo ci_instance{
    .pApplicationInfo = &i_application,
    .enabledLayerCount = static_cast<unsigned int>(layers.size()),
    .ppEnabledLayerNames = layers.data(),
    .enabledExtensionCount = static_cast<unsigned int>(extensions.size()),
    .ppEnabledExtensionNames = extensions.data()
  };
  if (hasPortabilityBit) ci_instance.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;

  vk_instance = vk_context.createInstance(ci_instance);
}

} // namespace pecs
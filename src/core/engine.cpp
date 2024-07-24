#include "src/core/include/engine.hpp"

#define VK_VALIDATION_LAYER_NAME "VK_LAYER_KHRONOS_validation"
#define VECS_ENGINE_VERSION       VK_MAKE_API_VERSION(0, 1, 2, 0)

namespace vecs
{

Engine::Engine()
{
  entity_manager = std::make_unique<EntityManager>();
  component_manager = std::make_shared<ComponentManager>();
  system_manager = std::make_unique<SystemManager>();
}

Engine::~Engine()
{
  entity_manager.reset();
  component_manager.reset();
  system_manager.reset();
  
  vecs_gui.reset();
  vecs_device.reset();

  Settings::destroy();
}

void Engine::load()
{
  initialize();
}

void Engine::run()
{
  while (!close_condition())
  {
    poll_gui();
  }
}

void Engine::createInstance()
{
  vk::raii::Context vk_context;

  std::string name = VECS_SETTINGS.name();
  vk::ApplicationInfo i_application{
    .pApplicationName   = name.c_str(),
    .applicationVersion = VECS_SETTINGS.version(),
    .pEngineName        = "VECS",
    .engineVersion      = VECS_ENGINE_VERSION,
    .apiVersion         = VK_API_VERSION_1_3
  };

  std::vector<const char *> layers;
  if (VECS_SETTINGS.validation_enabled()) layers.emplace_back(VK_VALIDATION_LAYER_NAME);
  
  auto extensions = vecs_gui->extensions();
  for (const auto& extension : vk_context.enumerateInstanceExtensionProperties())
  {
    if (std::string(extension.extensionName) == VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)
    {
      extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
      
      VECS_SETTINGS.toggle_portability();
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
  if (VECS_SETTINGS.portability_enabled()) ci_instance.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;

  vk_instance = vk_context.createInstance(ci_instance);
}

bool Engine::close_condition()
{
  return should_close();
}

bool Engine::should_close() const
{
  return vecs_gui->shouldClose();
}

void Engine::initialize(void * p_next)
{
  vecs_gui = std::make_shared<GUI>();
  createInstance();
  vecs_gui->createSurface(vk_instance);
  vecs_device = std::make_shared<Device>(vk_instance, *vecs_gui, p_next);
  vecs_gui->setupWindow(*vecs_device);
}

void Engine::poll_gui()
{
  vecs_gui->pollEvents();
}

} // namespace vecs

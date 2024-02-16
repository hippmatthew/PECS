/*
 *  PECS::core - engine.cpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  2/15/24
 */

#include "src/core/include/engine.hpp"

#define PECS_ENGINE_NAME "PECS"
#define PECS_ENGINE_VERSION VK_MAKE_API_VERSION(0, 1, 0, 0)

namespace pecs
{

Engine::Engine()
{
  Settings s;

  initialize(s);
}

Engine::Engine(const Settings& s)
{
  settings = s.engine;

  initialize(s);
}

Engine::~Engine()
{  
  for (auto * object : objects)
    object->clean();
  
  vk_flightFences.clear();
  vk_imageSemaphores.clear();
  vk_renderSemaphores.clear();
  
  delete renderer;
  delete gui;
  delete device;
}

const ViewportInfo Engine::viewportInfo() const
{
  ViewportInfo i_viewport;
  i_viewport.first = gui->extent();
  i_viewport.second = gui->format();

  return i_viewport; 
}

void Engine::run()
{ 
  renderer->setup(objects, *device);
  
  while (!glfwWindowShouldClose(gui->window()))
  {
    auto startFrame = std::chrono::high_resolution_clock::now();
    
    glfwPollEvents();

    static_cast<void>(device->logical().waitForFences({ *vk_flightFences[frameIndex] }, vk::True, UINT64_MAX));

    for (auto * object : objects)
    {
      if (object->hasTransformed)
      {
        object->objectData.model = object->nextTransformation; 
        object->hasTransformed = false;
      }
           
      object->objectData.view = camera.first;
      object->objectData.projection = camera.second;
    }
      
    auto result = gui->swapchain().acquireNextImage(UINT64_MAX, *vk_imageSemaphores[frameIndex], nullptr);
    if (result.first == vk::Result::eErrorOutOfDateKHR || result.first == vk::Result::eSuboptimalKHR)
    {
      gui->recreateSwapchain(device->physical(), device->logical());
      continue;
    }
    else if (result.first != vk::Result::eSuccess)
      throw std::runtime_error("error @ pecs::Engine::run() : failed to get next image");

    device->logical().resetFences({ *vk_flightFences[frameIndex] });
    
    renderer->render(objects, frameIndex, gui->image(result.second), gui->imageView(result.second), *device);
    
    std::vector<vk::Semaphore> waitSemaphores = { *vk_imageSemaphores[frameIndex] };
    std::vector<vk::Semaphore> signalSemaphores = { *vk_renderSemaphores[frameIndex] };
    std::vector<vk::PipelineStageFlags> waitStages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    std::vector<vk::CommandBuffer> commandBuffers = { *renderer->commandBuffers()[frameIndex] };
    
    vk::SubmitInfo i_submit{
      .waitSemaphoreCount = static_cast<unsigned int>(waitSemaphores.size()),
      .pWaitSemaphores    = waitSemaphores.data(),
      .pWaitDstStageMask  = waitStages.data(),
      .commandBufferCount = static_cast<unsigned int>(commandBuffers.size()),
      .pCommandBuffers    = commandBuffers.data(),
      .signalSemaphoreCount = static_cast<unsigned int>(signalSemaphores.size()),
      .pSignalSemaphores    = signalSemaphores.data()
    };
    device->queue(QueueType::Graphics).submit(i_submit, *vk_flightFences[frameIndex]);

    std::vector<vk::SwapchainKHR> vk_swapchains = { *(gui->swapchain()) };

    vk::PresentInfoKHR i_present{
      .waitSemaphoreCount = static_cast<unsigned int>(signalSemaphores.size()),
      .pWaitSemaphores    = signalSemaphores.data(),
      .swapchainCount     = static_cast<unsigned int>(vk_swapchains.size()),
      .pSwapchains        = vk_swapchains.data(),
      .pImageIndices      = &result.second
    };
    auto presentResult = device->queue(QueueType::Present).presentKHR(i_present);

    if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR)
      gui->recreateSwapchain(device->physical(), device->logical());
    else if (presentResult != vk::Result::eSuccess)
      throw std::runtime_error("error @ pecs::Engine::run() : failed to present image");
    
    Main();

    auto endFrame = std::chrono::high_resolution_clock::now();
    deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(endFrame - startFrame).count();

    frameIndex = (frameIndex + 1) % renderer->maxFlightFrames();
  }

  device->logical().waitIdle();
}

void Engine::addObject(Object * o)
{
  if (o->vertices.size() < 3) return;

  o->createGraphicsPipeline(device->logical(), viewportInfo());
  objects.emplace_back(o);
}

void Engine::initialize(const Settings& s)
{
  if (settings.layerBits & PECS_VALIDATION_BIT)
    layers.emplace_back("VK_LAYER_KHRONOS_validation");

  gui = new GUI(s.gui);

  createInstance();

  gui->createSurface(vk_instance);
  device = new Device(vk_instance, gui->surface());
  gui->setupWindow(device->physical(), device->logical());

  renderer = new Renderer(s.renderer, *device, viewportInfo());
  createSyncObjects();

  camera = Camera(
    glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    glm::perspective(glm::radians(45.0f), static_cast<float>(gui->extent().width / gui->extent().height), 0.1f, 10.0f)
  );
  camera.second[1][1] *= -1;
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
      
      hasPortabilityBit = true;
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
  if (hasPortabilityBit) ci_instance.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;

  vk_instance = vk_context.createInstance(ci_instance);
}

void Engine::createSyncObjects()
{
  vk::SemaphoreCreateInfo ci_semaphore;
  vk::FenceCreateInfo ci_fence{
    .flags = vk::FenceCreateFlagBits::eSignaled
  };

  for (std::size_t i = 0; i < renderer->maxFlightFrames(); ++i)
  {
    vk_imageSemaphores.emplace_back(device->logical().createSemaphore(ci_semaphore));
    vk_renderSemaphores.emplace_back(device->logical().createSemaphore(ci_semaphore));
    vk_flightFences.emplace_back(device->logical().createFence(ci_fence));
  }
}

} // namespace pecs
/*
*   PECS - engine.cpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    7/23/23
*/

#include "include/engine.hpp"

namespace pecs
{

Engine::Engine(const InitializationInfo * initInfo)
{
    initialize(initInfo);
}

Engine::~Engine()
{   
    if (debugManager->isEnabled()) debugManager->message("destroying device...");
    delete device;
    
    if (debugManager->isEnabled()) debugManager->message("destroying window surface...");
    window->destroySurface(instance);
    
    if (debugManager->isEnabled()) debugManager->message("destroying vulkan instance...");
    instance.destroy();

    if (debugManager->isEnabled()) debugManager->message("destroying window...");
    delete window;

    if (debugManager->isEnabled()) debugManager->message("disabling debug manager...");
    delete debugManager;
}

bool Engine::isActive() const
{ return !window->shouldClose(); }

void Engine::getEvents() const
{ glfwPollEvents(); }

void Engine::initialize(const InitializationInfo* initInfo)
{    
    if (initInfo->enableDebugManager)
    {
        debugManager = new DebugManager;
        debugManager->message("debug manager enabled");
    }
    else
        debugManager = new DebugManager(false);
    
    if (debugManager->isEnabled()) debugManager->message("creating window...");
    window = new Window(initInfo->windowWidth, initInfo->windowHeight, initInfo->windowTitle, debugManager);
    
    if (debugManager->isEnabled()) debugManager->message("creating vulkan instance...");
    createVulkanInstance(initInfo->applicationName, initInfo->applicationVersion, initInfo->engineName, initInfo->engineVersion);

    if (debugManager->isEnabled()) debugManager->message("creating window surface...");
    window->createSurface(instance);

    if (debugManager->isEnabled()) debugManager->message("creating device...");
    device = new Device(instance, window, debugManager);
}

void Engine::createVulkanInstance(const std::string& applicationName, const unsigned int& applicationVersion, const std::string& engineName, const unsigned int& engineVersion)
{
    vk::ApplicationInfo applicationInfo{ .pApplicationName      = applicationName.c_str(),
                                         .applicationVersion    = applicationVersion,
                                         .pEngineName           = engineName.c_str(),
                                         .engineVersion         = engineVersion,
                                         .apiVersion            = VK_API_VERSION_1_3 };

    vk::InstanceCreateInfo instanceCreateInfo{ .pApplicationInfo = &applicationInfo };

    if (debugManager->isEnabled()) debugManager->message("\tcollecting required extensions...");
    auto requiredExtensions = getRequiredExtensions();

    if (enumerateInstanceExtensions())
    {
        if (debugManager->isEnabled()) debugManager->message("\tenabling portability extensions...");
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        requiredExtensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

        instanceCreateInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
    }
    
    instanceCreateInfo.enabledExtensionCount = static_cast<unsigned int>(requiredExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
    instanceCreateInfo.enabledLayerCount = 0;
        
    instance = vk::createInstance(instanceCreateInfo);
    if (debugManager->isEnabled()) debugManager->message("\testablished vulkan instance");
     
}

bool Engine::enumerateInstanceExtensions() const
{   
    auto extensionProperties = vk::enumerateInstanceExtensionProperties();

    if (debugManager->isEnabled()) debugManager->message("\tavailable extensions:");

    bool hasPortabilityBit = false;
    for (const auto& property : extensionProperties)
    {
        if (debugManager->isEnabled()) debugManager->message("\t\t" + static_cast<std::string>(property.extensionName));
        if (strcmp(property.extensionName, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0)
            hasPortabilityBit = true;
    }

    return hasPortabilityBit;
}

std::vector<const char *> Engine::getRequiredExtensions() const
{
    unsigned int glfwExtensionCount = 0;
    const char ** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}

}
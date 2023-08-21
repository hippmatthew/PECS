/*
*   PECS - engine.cpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    7/21/23
*/

#include "include/engine.hpp"

namespace pecs
{

Engine::~Engine()
{
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
    createVulkanInstance(initInfo->applicationName, initInfo->applicationVersion);
}

void Engine::createVulkanInstance(std::string applicationName, unsigned int applicationVersion)
{
    vk::ApplicationInfo applicationInfo{ .pApplicationName      = applicationName.c_str(),
                                         .applicationVersion    = applicationVersion,
                                         .pEngineName           = engineInfo.name.c_str(),
                                         .engineVersion         = engineInfo.version,
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
        
    vk::Result result = vk::createInstance(&instanceCreateInfo, nullptr, &instance);

    if (result != vk::Result::eSuccess) debugManager->message(result);
    if (debugManager->isEnabled()) debugManager->message("\testablished vulkan instance");
     
}

bool Engine::enumerateInstanceExtensions() const
{   
    unsigned int extensionCount = 0;
    static_cast<void>(vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));

    std::vector<vk::ExtensionProperties> extensions(extensionCount);
    vk::Result result = vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    
    switch (result)
    {
        case vk::Result::eSuccess:
            if (debugManager->isEnabled()) debugManager->message("\tavailable extensions:");
            break;
        case vk::Result::eIncomplete:
            if (debugManager->isEnabled()) debugManager->message("\twarning: not all extensions were available\n\tavailable extensions:");
            break;
        default:
            debugManager->message(result);
    }

    bool hasPortabilityBit = false;
    for (const auto& extension : extensions)
    {
        if (debugManager->isEnabled()) debugManager->message("\t\t" + static_cast<std::string>(extension.extensionName));
        if (strcmp(extension.extensionName, "VK_KHR_portability_enumeration") == 0)
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
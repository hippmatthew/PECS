/*
*   PECS - engine.cpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    6/29/23
*/

#include "include/engine.hpp"

namespace pecs
{

Engine::~Engine()
{
    instance.destroy();
    delete window;
}

bool Engine::isActive() const
{ return !window->shouldClose(); }

Window* Engine::getWindow() const
{ return window; }

void Engine::initialize(const InitializationInfo* initInfo)
{
    window = new Window(initInfo->windowWidth, initInfo->windowHeight, initInfo->windowTitle);
    
    createVulkanInstance(initInfo->applicationName, initInfo->applicationVersion);
    
    if (debugManager->isEnabled()) debugManager->setupDebugMessenger(instance);
}

void Engine::createVulkanInstance(std::string applicationName, unsigned int applicationVersion)
{
    if (debugManager->isEnabled() && !debugManager->checkValidationLayerSupport())
        throw std::runtime_error("not all validation layers are supported\n");

    vk::ApplicationInfo applicationInfo{ .pApplicationName      = applicationName.c_str(),
                                         .applicationVersion    = applicationVersion,
                                         .pEngineName           = engineInfo.name.c_str(),
                                         .engineVersion         = engineInfo.version,
                                         .apiVersion            = VK_API_VERSION_1_3 };

    vk::InstanceCreateInfo instanceCreateInfo{ .pApplicationInfo = &applicationInfo };

    auto requiredExtensions = getRequiredExtensions();

    if (enumerateInstanceExtensions())
    {
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        requiredExtensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

        instanceCreateInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;

        if (debugManager->isEnabled()) std::cout << "enabled portability enumeration bit\n";
    }
    
    instanceCreateInfo.enabledExtensionCount = static_cast<unsigned int>(requiredExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
    
    if (debugManager->isEnabled())
    {
        instanceCreateInfo.enabledLayerCount = static_cast<unsigned int>(debugManager->getValidationLayers().size());
        instanceCreateInfo.ppEnabledLayerNames = debugManager->getValidationLayers().data();
    }
    else
        instanceCreateInfo.enabledLayerCount = 0;
    
    if (vk::createInstance(&instanceCreateInfo, nullptr, &instance) != vk::Result::eSuccess)
        throw std::runtime_error("failed to create instance");
}

bool Engine::enumerateInstanceExtensions() const
{   
    unsigned int extensionCount = 0;
    static_cast<void>(vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));

    std::vector<vk::ExtensionProperties> extensions(extensionCount);
    switch(vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data()))
    {
        case vk::Result::eSuccess:
        case vk::Result::eIncomplete:
            break;
        case vk::Result::eErrorOutOfDeviceMemory:
            throw std::runtime_error("error: out of device memory\n");
        case vk::Result::eErrorOutOfHostMemory:
            throw std::runtime_error("error: out of host memory\n");
        default:
            throw std::runtime_error("error: layer not present\n");
    }

    for (const auto& extension : extensions)
        if (strcmp(extension.extensionName, "VK_KHR_portability_enumeration") == 0)
            return true;

    return false;
}

std::vector<const char *> Engine::getRequiredExtensions() const
{
    unsigned int glfwExtensionCount = 0;
    const char ** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (debugManager->isEnabled())
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

}
/*
*   PECS - engine.cpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    6/29/23
*/

#include "include/engine.hpp"

namespace pecs
{

Engine::Engine() {}

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
}

void Engine::createVulkanInstance(std::string applicationName, unsigned int applicationVersion)
{
    vk::ApplicationInfo applicationInfo{ .pApplicationName      = applicationName.c_str(),
                                         .applicationVersion    = applicationVersion,
                                         .pEngineName           = engineInfo.name.c_str(),
                                         .engineVersion         = engineInfo.version,
                                         .apiVersion            = VK_API_VERSION_1_3 };

    vk::InstanceCreateInfo instanceCreateInfo{ .pApplicationInfo = &applicationInfo };

    bool hasPortabilityExtension = enumerateInstanceExtensions();

    unsigned int glfwExtensionCount = 0;
    const char ** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> requiredExtensions;

    for (unsigned int i = 0; i < glfwExtensionCount; ++i)
        requiredExtensions.emplace_back(glfwExtensions[i]);

    if (hasPortabilityExtension)
    {
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        requiredExtensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

        instanceCreateInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;

        std::cout << "portability enumeration bit enabled\n";
    }

    instanceCreateInfo.enabledExtensionCount = static_cast<unsigned int>(requiredExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
    instanceCreateInfo.enabledLayerCount = 0;
    
    if (vk::createInstance(&instanceCreateInfo, nullptr, &instance) != vk::Result::eSuccess)
        throw std::runtime_error("failed to create instance");

    std::cout << "instance created\n";
}

bool Engine::enumerateInstanceExtensions() const
{
    unsigned int extensionCount = 0;
    vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<vk::ExtensionProperties> extensions(extensionCount);
    switch(vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data()))
    {
        case vk::Result::eSuccess:
        case vk::Result::eIncomplete:
            break;
        case vk::Result::eErrorOutOfDeviceMemory:
            throw std::runtime_error("out of device memory");
        case vk::Result::eErrorOutOfHostMemory:
            throw std::runtime_error("out of host memory");
        default:
            throw std::runtime_error("failed to gather instance extensions");
    }

    for (const auto& extension : extensions)
        if (strcmp(extension.extensionName, "VK_KHR_portability_enumeration") == 0)
            return true;

    return false;

}

}
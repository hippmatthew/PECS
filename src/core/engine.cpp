/*
*   PECS - engine.cpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    10/30/23
*/

#include "./include/engine.hpp"

namespace pecs
{

Engine::Engine(const InitializationInfo * initInfo)
{
    initialize(initInfo);
}

Engine::~Engine()
{   
    std::cout << "\n\n---cleaning engine---\n\n"; 
    
    std::cout << "destroying device...\n";
    delete device;
    
    std::cout << "destroying window surface...\n";
    window->destroySurface(instance);
    
    std::cout << "destroying vulkan instance...\n";
    instance.destroy();

    std::cout << "destroying window...\n";
    delete window;
}

void Engine::initialize(const InitializationInfo* initInfo)
{    
    std::cout << "\n---initializing PECS engine---\n\n";
    
    std::cout << "creating window...\n";
    window = new Window(initInfo->windowWidth, initInfo->windowHeight, initInfo->windowTitle);
    
    std::cout << "creating vulkan instance...\n";
    createVulkanInstance(initInfo->applicationName, initInfo->applicationVersion);

    std::cout << "creating window surface...\n";
    window->createSurface(instance);

    std::cout << "creating device...\n";
    device = new Device(instance, window);
}

void Engine::run()
{
    std::cout << "\n---running main loop---\n\n";
    while (!window->shouldClose())
    {
        glfwPollEvents();

        // run compute stage
        // run graphics stage
    }
}

void Engine::run(Main& mainLoop)
{
    std::cout << "\n---running main loop---\n\n";
    while (!window->shouldClose())
    {
        glfwPollEvents();

        // run compute stage
        // run graphics stage

        mainLoop();
    }
}

void Engine::createVulkanInstance(const std::string& applicationName, const unsigned int& applicationVersion)
{
    vk::ApplicationInfo applicationInfo{ .pApplicationName      = applicationName.c_str(),
                                         .applicationVersion    = applicationVersion,
                                         .pEngineName           = "PECS",
                                         .engineVersion         = VK_MAKE_API_VERSION(0, 1, 0, 0),
                                         .apiVersion            = VK_API_VERSION_1_3 };

    if (enableValidationLayers)
    {
        std::cout << "\tusing validation layers\n";
        layers.push_back("VK_LAYER_KHRONOS_validation");
    }
    
    vk::InstanceCreateInfo instanceCreateInfo{ .pApplicationInfo = &applicationInfo,
                                               .enabledLayerCount   = static_cast<unsigned int>(layers.size()),
                                               .ppEnabledLayerNames = layers.data() };

    std::cout << "\tcollecting required extensions...\n";
    auto requiredExtensions = getRequiredExtensions();

    if (enumerateInstanceExtensions())
    {
        std::cout << "\tenabling portability extensions...\n";
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        requiredExtensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

        instanceCreateInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
    }
    
    instanceCreateInfo.enabledExtensionCount = static_cast<unsigned int>(requiredExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
    instanceCreateInfo.enabledLayerCount = 0;
        
    instance = vk::createInstance(instanceCreateInfo);
    std::cout << "\testablished vulkan instance\n";
     
}

bool Engine::enumerateInstanceExtensions() const
{   
    auto extensionProperties = vk::enumerateInstanceExtensionProperties();

    std::cout << "\tavailable extensions:\n";

    bool hasPortabilityBit = false;
    for (const auto& property : extensionProperties)
    {
        std::cout << "\t\t" << static_cast<std::string>(property.extensionName) << '\n';
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
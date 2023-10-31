/*
*   PECS - engine.cpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    10/31/23
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
    
    std::cout << "detroying swapchain image views...\n";
    for (vk::ImageView imageView : swapchainImageViews)
        device->getLogicalDevice().destroyImageView(imageView);
    
    std::cout << "destroying swapchain...\n";
    device->getLogicalDevice().destroySwapchainKHR(swapchain);
    
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
    std::cout << "\n---initializing engine---\n\n";
    
    std::cout << "creating window...\n";
    window = new Window(initInfo->windowWidth, initInfo->windowHeight, initInfo->windowTitle);
    
    std::cout << "creating vulkan instance...\n";
    createVulkanInstance(initInfo->applicationName, initInfo->applicationVersion);

    std::cout << "creating window surface...\n";
    window->createSurface(instance);
    
    std::cout << "creating device...\n";
    device = new Device(instance, window);

    std::cout << "creating swapchain...\n";
    createSwapchain();

    std::cout << "creating swapchain image views\n";
    createImageViews();
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

void Engine::createSwapchain()
{
    SwapchainSupportDetails details = device->querySwapchainSupport(device->getPhysicalDevice(), window->getSurface());

    std::cout << "\tgetting surface format...\n";
    vk::SurfaceFormatKHR surfaceFormat = chooseSwapchainSurfaceFormat(details.formats);

    std::cout << "\tchoosing presentation mode...\n";
    vk::PresentModeKHR presentMode = chooseSwapchainPresentMode(details.presentModes);

    std::cout << "\tsetting swap extent...\n";
    vk::Extent2D extent = chooseSwapchainExtent(details.surfaceCapabilities, window->getGLFWwindow());

    unsigned int imageCount = details.surfaceCapabilities.minImageCount + 1;
    if (details.surfaceCapabilities.maxImageCount > 0 && imageCount > details.surfaceCapabilities.maxImageCount)
        imageCount = details.surfaceCapabilities.maxImageCount;

    vk::SwapchainCreateInfoKHR swapchainCreateInfo{ .surface            = window->getSurface(),
                                                    .minImageCount      = imageCount,
                                                    .imageFormat        = surfaceFormat.format,
                                                    .imageColorSpace    = surfaceFormat.colorSpace,
                                                    .imageExtent        = extent,
                                                    .imageArrayLayers   = 1,
                                                    .imageUsage         = vk::ImageUsageFlagBits::eColorAttachment,
                                                    .preTransform       = details.surfaceCapabilities.currentTransform,
                                                    .compositeAlpha     = vk::CompositeAlphaFlagBitsKHR::eOpaque,
                                                    .presentMode        = presentMode,
                                                    .clipped            = VK_TRUE,
                                                    .oldSwapchain       = VK_NULL_HANDLE};

    QueueFamilyIndices indices = device->findPhysicalDeviceQueueFamilyIndicies(device->getPhysicalDevice(), window->getSurface());
    
    if (indices.graphics != indices.present)
    {
        unsigned int indicesArray[3] = { indices.graphics.value(), indices.present.value(), indices.compute.value() };
        
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchainCreateInfo.queueFamilyIndexCount = 3;
        swapchainCreateInfo.pQueueFamilyIndices = indicesArray;
    }
    else
    {
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    swapchain = device->getLogicalDevice().createSwapchainKHR(swapchainCreateInfo);
    std::cout << "\tswapchain created\n";

    swapchainImageDetails.extent = extent;
    swapchainImageDetails.format = surfaceFormat.format;
    std::cout << "\tsaved swapchain extent and surface format\n";

    swapchainImages = device->getLogicalDevice().getSwapchainImagesKHR(swapchain);
    std::cout << "\tretrieved swapchain images\n";
}

void Engine::createImageViews()
{
    swapchainImageViews.resize(swapchainImages.size());
    
    vk::ComponentMapping components{ .r = vk::ComponentSwizzle::eIdentity,
                                     .g = vk::ComponentSwizzle::eIdentity,
                                     .b = vk::ComponentSwizzle::eIdentity,
                                     .a = vk::ComponentSwizzle::eIdentity };

    vk::ImageSubresourceRange subresourceRange{ .aspectMask     = vk::ImageAspectFlagBits::eColor,
                                                .baseMipLevel   = 0,
                                                .levelCount     = 1,
                                                .baseArrayLayer = 0,
                                                .layerCount     = 1 };
    
    unsigned int imageCount = 0;
    for (vk::Image image : swapchainImages)
    {
        vk::ImageViewCreateInfo imageViewCreateInfo{ .image             = image,
                                                     .viewType          = vk::ImageViewType::e2D,
                                                     .format            = swapchainImageDetails.format,
                                                     .components        = components,
                                                     .subresourceRange  = subresourceRange };
        
        swapchainImageViews[imageCount++] = device->getLogicalDevice().createImageView(imageViewCreateInfo);
    }
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

vk::SurfaceFormatKHR Engine::chooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const
{
    for (const auto& format : availableFormats)
        if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            return format;

    return availableFormats[0];
}

vk::PresentModeKHR Engine::chooseSwapchainPresentMode(const std::vector<vk::PresentModeKHR>& availableModes) const
{
    for (const auto& mode : availableModes)
        if (mode == vk::PresentModeKHR::eMailbox)
            return mode;

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D Engine::chooseSwapchainExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const GLFWwindow * window) const
{
    if (capabilities.currentExtent.width != std::numeric_limits<unsigned int>::max())
        return capabilities.currentExtent;

    int width, height;
    glfwGetFramebufferSize(const_cast<GLFWwindow *>(window), &width, &height);

    vk::Extent2D extent{ .width     = static_cast<unsigned int>(width),
                         .height    = static_cast<unsigned int>(height) };

    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return extent;
}

}
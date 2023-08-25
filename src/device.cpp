/*
 *  PECS - device.cpp
 *  Author:     Matthew Hipp
 *  Created:    7/21/23
 *  Updated:    7/25/23
 */

#include "include/device.hpp"

namespace pecs
{

Device::Device(const vk::Instance& instance, const Window * window, const DebugManager * dm) : debugManager(dm)
{
    if (debugManager->isEnabled()) debugManager->message("\tpicking physical device...");
    choosePhysicalDevice(instance, window->getSurface());

    if (debugManager->isEnabled()) debugManager->message("\tcreating logical device...");
    createLogicalDevice(window->getSurface());

    if (debugManager->isEnabled()) debugManager->message("\tcreating swapchain...");
    createSwapchain(window);

}

Device::~Device()
{
    if (debugManager->isEnabled()) debugManager->message("\tdestroying swapchain...");
    logicalDevice.destroySwapchainKHR(swapchain);

    if (debugManager->isEnabled()) debugManager->message("\tdestroying logical device...");
    logicalDevice.destroy();
}

const vk::PhysicalDevice& Device::getPhysicalDevice() const
{ return physicalDevice; }

vk::PhysicalDeviceProperties Device::getPhysicalDeviceProperties() const
{ return physicalDevice.getProperties(); }

const vk::Queue& Device::getQueue(QueueType type) const
{
    switch (type)
    {
        case Graphics:
            return graphicsQueue;
        case Present:
            return presentQueue;
        case Compute:
            return computeQueue;
    }
}

void Device::choosePhysicalDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface)
{
    auto devices = instance.enumeratePhysicalDevices();

    auto suitableDevices = getSuitablePhysicalDevices(devices, surface);
    if (suitableDevices.size() == 0) debugManager->message("no suitable physical devices found", true);

    if (debugManager->isEnabled())
    {
        for (const auto& device : suitableDevices)
        {
            auto properties = device.getProperties();
            debugManager->message("\t\t\tname: " + static_cast<std::string>(properties.deviceName) + "\ttype: " + vk::to_string(properties.deviceType));
        }
    }
    
    physicalDevice = suitableDevices[0];
    
    if (debugManager->isEnabled())
    {
        auto properties = physicalDevice.getProperties();
        debugManager->message("\t\tchosen physical device: " + static_cast<std::string>(properties.deviceName) + "\n\t\tavailable physical device extensions:");

        auto extensionProperties = physicalDevice.enumerateDeviceExtensionProperties();
        for (const auto& property : extensionProperties)
        {
            std::string output = "\t\t\t" + static_cast<std::string>(property.extensionName);
            
            for (const auto& extension : deviceExtensions)
                if (strcmp(extension, property.extensionName) == 0)
                    output += " (required)";

            debugManager->message(output);
        }
    }
}

void Device::createLogicalDevice(const vk::SurfaceKHR& surface)
{
    QueueFamilyIndices indices = findPhysicalDeviceQueueFamilyIndicies(physicalDevice, surface);

    float graphicsQueuePriority = 1.0f;
    vk::DeviceQueueCreateInfo graphicsQueueCreateInfo{ .queueFamilyIndex    = indices.graphics.value(),
                                                       .queueCount          = 1,
                                                       .pQueuePriorities    = &graphicsQueuePriority };

    float presentQueuePriority = 1.0f;
    vk::DeviceQueueCreateInfo presentQueueCreateInfo{ .queueFamilyIndex = indices.present.value(),
                                                      .queueCount       = 1,
                                                      .pQueuePriorities = &presentQueuePriority };

    float computeQueuePriority = 1.0f;
    vk::DeviceQueueCreateInfo computeQueueCreateInfo{ .queueFamilyIndex = indices.compute.value(),
                                                      .queueCount       = 1,
                                                      .pQueuePriorities = &computeQueuePriority };

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos = { graphicsQueueCreateInfo, presentQueueCreateInfo, computeQueueCreateInfo };

    vk::PhysicalDeviceFeatures physicalDeviceFeatures{};

    vk::DeviceCreateInfo deviceCreateInfo{ .queueCreateInfoCount    = static_cast<unsigned int>(queueCreateInfos.size()),
                                           .pQueueCreateInfos       = queueCreateInfos.data(),
                                           .enabledLayerCount       = 0,
                                           .enabledExtensionCount   = static_cast<unsigned int>(deviceExtensions.size()),
                                           .ppEnabledExtensionNames = deviceExtensions.data(),
                                           .pEnabledFeatures        = &physicalDeviceFeatures };

    logicalDevice = physicalDevice.createDevice(deviceCreateInfo);
    if (debugManager->isEnabled()) debugManager->message("\t\tcreated logical device");
    
    logicalDevice.getQueue(indices.graphics.value(), 0, &graphicsQueue);
    if (debugManager->isEnabled()) debugManager->message("\t\testablished graphics queue");

    logicalDevice.getQueue(indices.present.value(), 0, &presentQueue);
    if (debugManager->isEnabled()) debugManager->message("\t\testablished present queue");

    logicalDevice.getQueue(indices.compute.value(), 0, &computeQueue);
    if (debugManager->isEnabled()) debugManager->message("\t\testablished compute queue");
}

void Device::createSwapchain(const Window * window)
{
    SwapchainSupportDetails details = querySwapchainSupport(physicalDevice, window->getSurface());

    if (debugManager->isEnabled()) debugManager->message("\t\tgetting surface format...");
    vk::SurfaceFormatKHR surfaceFormat = chooseSwapchainSurfaceFormat(details.formats);

    if (debugManager->isEnabled()) debugManager->message("\t\tchoosing presentation mode...");
    vk::PresentModeKHR presentMode = chooseSwapchainPresentMode(details.presentModes);

    if (debugManager->isEnabled()) debugManager->message("\t\tsetting swap extent...");
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

    QueueFamilyIndices indices = findPhysicalDeviceQueueFamilyIndicies(physicalDevice, window->getSurface());
    
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

    swapchain = logicalDevice.createSwapchainKHR(swapchainCreateInfo);
    if (debugManager->isEnabled()) debugManager->message("\tswapchain created");
}

std::vector<vk::PhysicalDevice> Device::getSuitablePhysicalDevices(const std::vector<vk::PhysicalDevice>& devices, const vk::SurfaceKHR& surface) const
{
    std::vector<vk::PhysicalDevice> discreteGpus, integratedGpus, virtualGpus, sortedGpus;

    for (auto& device : devices)
    {
        auto properties = device.getProperties();

        QueueFamilyIndices indices = findPhysicalDeviceQueueFamilyIndicies(device, surface);
        if (!indices.isComplete()) continue;

        if (!checkPhysicalDeviceExtensionSupport(device)) continue;

        auto swapchainSupport = querySwapchainSupport(device, surface);
        
        if (swapchainSupport.formats.empty()) continue;
        if (swapchainSupport.presentModes.empty()) continue;

        switch (evaluate(properties.deviceType))
        {
            case 3:
                discreteGpus.push_back(device);
                continue;
            case 2:
                integratedGpus.push_back(device);
                continue;
            case 1:
                virtualGpus.push_back(device);
                continue;
            case 0:
                continue;
        }
    }
    
    for (const auto& device : discreteGpus)
        sortedGpus.push_back(device);

    for (const auto& device : integratedGpus)
        sortedGpus.push_back(device);

    for (const auto& device : virtualGpus)
        sortedGpus.push_back(device);

    return sortedGpus;
}

QueueFamilyIndices Device::findPhysicalDeviceQueueFamilyIndicies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) const
{
    QueueFamilyIndices indices;

    auto queueFamilies = device.getQueueFamilyProperties();
    
    unsigned int i = 0;
    for (auto family : queueFamilies)
    {   
        if (family.queueFlags & vk::QueueFlagBits::eGraphics)
            indices.graphics = i;
        
        if (family.queueFlags & vk::QueueFlagBits::eCompute)
            indices.compute = i;

        if (indices.isComplete())
            break;

        vk::Bool32 presentQueueSupport = false;
        vk::Result result = device.getSurfaceSupportKHR(i, surface, &presentQueueSupport);
        if (result != vk::Result::eSuccess) debugManager->message(result);

        if (presentQueueSupport)
            indices.present = i;

        ++i;
    }

    return indices;
}

unsigned int Device::evaluate(const vk::PhysicalDeviceType type) const
{
    switch (type)
    {
        case vk::PhysicalDeviceType::eDiscreteGpu:
            return 3;
        case vk::PhysicalDeviceType::eIntegratedGpu:
            return 2;
        case vk::PhysicalDeviceType::eVirtualGpu:
            return 1;
        default:
            return 0;
    }
}

bool Device::checkPhysicalDeviceExtensionSupport(const vk::PhysicalDevice& device) const
{
    auto extensionProperties = device.enumerateDeviceExtensionProperties();
    
    std::vector<bool> supported;
    for (const auto& extension : deviceExtensions)
    {
        bool support = false;
        
        for (const auto& property : extensionProperties)
            if (strcmp(property.extensionName, extension) == 0)
                support = true;

        supported.push_back(support);
    }

    for (const auto& support : supported)
        if (!support) return false;

    return true;
}

SwapchainSupportDetails Device::querySwapchainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) const
{
    SwapchainSupportDetails details;
    
    if (debugManager->isEnabled()) debugManager->message("\tgetting surface capabilities...");
    details.surfaceCapabilities = device.getSurfaceCapabilitiesKHR(surface);

    if (debugManager->isEnabled()) debugManager->message("\tgetting surface formats...");
    details.formats = device.getSurfaceFormatsKHR(surface);

    if (debugManager->isEnabled()) debugManager->message("\tgetting surface presentation modes...");
    details.presentModes = device.getSurfacePresentModesKHR(surface);

    return details;
}

vk::SurfaceFormatKHR Device::chooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const
{
    for (const auto& format : availableFormats)
        if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            return format;

    return availableFormats[0];
}

vk::PresentModeKHR Device::chooseSwapchainPresentMode(const std::vector<vk::PresentModeKHR>& availableModes) const
{
    for (const auto& mode : availableModes)
        if (mode == vk::PresentModeKHR::eMailbox)
            return mode;

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D Device::chooseSwapchainExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const GLFWwindow * window) const
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
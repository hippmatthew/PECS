/*
*   PECS - structs.hpp
*   Author:     Matthew Hipp
*   Created:    11/15/23
*   Updated:    11/15/23
*/

#ifndef pecs_structs_hpp
#define pecs_structs_hpp

#define VK_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace pecs
{
    struct InitializationInfo
{   
    std::string applicationName = "PECS Application";
    unsigned int applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

    std::string windowTitle = "PECS Application";
    unsigned int windowWidth = 600, windowHeight = 600;
};

struct QueueFamilyIndices
{
    std::optional<unsigned int> graphics;
    std::optional<unsigned int> present;
    std::optional<unsigned int> compute;

    bool isComplete() const
    { return graphics.has_value() && present.has_value() && compute.has_value(); }
};

struct SwapchainSupportDetails
{
    vk::SurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

struct SwapchainImageDetails
{
    vk::Extent2D extent;
    vk::Format format;
};

struct ShaderPaths
{
    std::string vertex = "";
    std::string fragment = "";
    std::string compute = "";
};

}

#endif /* pecs_structs_hpp */
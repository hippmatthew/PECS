/*
*   PECS - engine.hpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    10/31/23
*/

#ifndef pecs_engine_hpp
#define pecs_engine_hpp

#include "device.hpp"

namespace pecs
{

struct InitializationInfo
{
    std::string applicationName = "PECS Application";
    unsigned int applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

    std::string windowTitle = "PECS Application";
    unsigned int windowWidth = 600, windowHeight = 600;
};

struct SwapchainImageDetails
{
    vk::Extent2D extent;
    vk::Format format;
};

class Engine
{
    public:
        class Main
        {
            public:
                virtual void operator()() = 0;
        };
    
    public:
        Engine() = default;
        Engine(const InitializationInfo * initInfo);
        Engine(const Engine&) = delete;
        Engine(Engine&&) = delete;
        
        ~Engine();

        Engine& operator=(const Engine&) = delete;
        Engine& operator=(Engine&&) = delete;

        void initialize(const InitializationInfo * initInfo);
        void run();
        void run(Main& mainLoop);

    private:
        Window * window = nullptr;
        Device * device = nullptr;
        
        std::vector<const char *> layers;
        
        SwapchainImageDetails swapchainImageDetails;
        std::vector<vk::Image> swapchainImages;
        
        #ifdef NDEBUG
            const bool enableValidationLayers = false;
        #else
            const bool enableValidationLayers = true;
        #endif

        vk::Instance instance = VK_NULL_HANDLE;
        vk::SwapchainKHR swapchain = VK_NULL_HANDLE;
        std::vector<vk::ImageView> swapchainImageViews;

        void createVulkanInstance(const std::string& applicationName, const unsigned int& applicationVersion);
        void createSwapchain();
        void createImageViews();

        bool enumerateInstanceExtensions() const;
        std::vector<const char *> getRequiredExtensions() const;
        vk::SurfaceFormatKHR chooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats) const;
        vk::PresentModeKHR chooseSwapchainPresentMode(const std::vector<vk::PresentModeKHR>& availableModes) const;
        vk::Extent2D chooseSwapchainExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const GLFWwindow * window) const;
};

}

#endif /* pecs_engine_hpp */
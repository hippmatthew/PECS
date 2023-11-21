/*
*   PECS - engine.hpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    11/15/23
*/

#ifndef pecs_engine_hpp
#define pecs_engine_hpp

namespace pecs
{
    struct InitializationInfo;
    struct SwapchainImageDetails;
    
    class Engine;
    class Object;
}

#include "device.hpp"
#include "../../objects/include/object.hpp"

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
                Main(const Engine& engine) : device(engine.getDevice()), details(engine.swapchainImageDetails) {}
                
                ~Main()
                {
                    for (int i = 0; i < objects.size(); ++i)
                        objects[i].destroy(device);
                }

                virtual void operator()() = 0;

                void loadObjects()
                {        
                    for (int i = 0; i < objects.size(); ++i)
                    {
                        std::cout << "\tloading object " << i + 1 << "/" << objects.size() << "...\n";
                        if (0x10 & objects[i].pipelineTypes)
                            objects[i].graphicsPipeline = new GraphicsPipeline(device, details, objects[i].shaderPaths);

                        if (objects[i].graphicsPipeline != nullptr)
                            std::cout << "\t\tcreated graphics pipeline for object " << i << '\n';
                    }            
                }

            public:
                std::vector<Object> objects;

            private:
                const Device * device;
                const SwapchainImageDetails details;
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
        void run(Main&);

        const Device * getDevice() const;

    public:
        SwapchainImageDetails swapchainImageDetails;

    private:
        Window * window = nullptr;
        Device * device = nullptr;
        
        std::vector<const char *> layers;
        
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
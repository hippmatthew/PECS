/*
*   PECS - engine.hpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    10/30/23
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
        
        const std::vector<const char *> layers = { "VK_LAYER_KHRONOS_validation" };

        vk::Instance instance;

        void createVulkanInstance(const std::string& applicationName, const unsigned int& applicationVersion);

        bool enumerateInstanceExtensions() const;
        std::vector<const char *> getRequiredExtensions() const;
};

}

#endif /* pecs_engine_hpp */
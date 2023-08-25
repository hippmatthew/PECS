/*
*   PECS - engine.hpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    7/25/23
*/

#ifndef pecs_engine_hpp
#define pecs_engine_hpp

#include "device.hpp"
#include "window.hpp"

namespace pecs
{

struct InitializationInfo
{
    std::string engineName = "PECS";
    unsigned int engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    
    std::string applicationName = "PECS Application";
    unsigned int applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

    std::string windowTitle = "PECS Application";
    unsigned int windowWidth = 600, windowHeight = 600;

    bool enableDebugManager = true;
};

class Engine
{
    public:
        Engine() = default;
        Engine(const InitializationInfo * initInfo);
        Engine(const Engine&) = delete;
        
        ~Engine();

        Engine& operator=(const Engine&) = delete;

        bool isActive() const;
        void getEvents() const;

        void initialize(const InitializationInfo * initInfo);

    private:
        Window * window = nullptr;
        DebugManager * debugManager = nullptr;
        Device * device = nullptr;
        
        vk::Instance instance;

        void createVulkanInstance(const std::string& applicationName, const unsigned int& applicationVersion, const std::string& engineName, const unsigned int& engineVersion);

        bool enumerateInstanceExtensions() const;
        std::vector<const char *> getRequiredExtensions() const;
};

}

#endif /* pecs_engine_hpp */
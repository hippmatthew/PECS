/*
*   PECS - engine.hpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    7/22/23
*/

#ifndef pecs_engine_hpp
#define pecs_engine_hpp

#include "window.hpp"
#include "device.hpp"

namespace pecs
{

struct InitializationInfo
{
    std::string applicationName = "PECS Application";
    unsigned int applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

    std::string windowTitle = "PECS Application";
    unsigned int windowWidth = 600, windowHeight = 600;

    bool enableDebugManager = true;
};

struct EngineInfo
{
    std::string name;
    unsigned int version;
};

class Engine
{
    public:
        Engine() = default;
        Engine(const Engine&) = delete;
        ~Engine();

        Engine& operator=(const Engine&) = delete;

        bool isActive() const;
        void getEvents() const;

        void initialize(const InitializationInfo* initInfo);

    private:
        const EngineInfo engineInfo{ .name = "PECS",
                                     .version = VK_MAKE_API_VERSION(0, 1, 0, 0) };
        
        Window * window = nullptr;
        DebugManager * debugManager = nullptr;
        Device * device = nullptr;
        
        vk::Instance instance;

        void createVulkanInstance(std::string applicationName, unsigned int applicationVersion);

        bool enumerateInstanceExtensions() const;
        std::vector<const char *> getRequiredExtensions() const;
};

}

#endif /* pecs_engine_hpp */
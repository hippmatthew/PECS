/*
*   PECS - engine.hpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    6/27/23
*/

#ifndef pecs_engine_hpp
#define pecs_engine_hpp

#include <vector>
#include <stdexcept>
#include <cstring>
#include <iostream>

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include "window.hpp"

namespace pecs
{

struct InitializationInfo
{
    std::string applicationName = "PECS Application";
    unsigned int applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

    std::string windowTitle = "PECS Application";
    unsigned int windowWidth = 600, windowHeight = 600;
};

struct EngineInfo
{
    std::string name;
    unsigned int version;
};

class Engine
{
    public:
        Engine();
        Engine(const Engine&) = delete;
        ~Engine();

        Engine& operator=(const Engine&) = delete;

        bool isActive() const;
        Window* getWindow() const;

        void initialize(const InitializationInfo* initInfo);

    private:
        const EngineInfo engineInfo{ .name = "PECS",
                                     .version = VK_MAKE_API_VERSION(0, 1, 0, 0) };
        
        Window * window;
        vk::Instance instance;

        void createVulkanInstance(std::string applicationName, unsigned int applicationVersion);

        bool enumerateInstanceExtensions() const;
};

}

#endif /* pecs_engine_hpp */
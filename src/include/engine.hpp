/*
*   PECS - engine.hpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    6/27/23
*/

#ifndef pecs_engine_hpp
#define pecs_engine_hpp

#include <vulkan/vulkan.hpp>

#include "window.hpp"

namespace pecs
{

struct InitializationInfo
{
    std::string applicationName;
    unsigned int applicationVersion;

    std::string windowTitle;
    unsigned int windowWidth, windowHeight;
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

        bool IsActive() const;
        Window* GetWindow() const;

        void Initialize(const InitializationInfo* initInfo);

    private:
        const EngineInfo engineInfo{ .name = "PECS",
                                     .version = VK_MAKE_API_VERSION(0, 1, 0, 0) };
        
        Window * window;
};

}

#endif /* pecs_engine_hpp */
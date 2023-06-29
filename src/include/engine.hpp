/*
*   PECS - engine.hpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    6/27/23
*/

#include <vulkan/vulkan.hpp>

#include "window.hpp"

namespace pecs
{

struct ApplicationInfo
{
    const char * applicationName;
    uint32_t applicationVersion;

    unsigned int windowWidth, windowHeight;
    const char * windowTitle;
};

struct EngineInfo
{
    const char * name;
    uint32_t version;
};

class Engine
{
    public:
        Engine(const ApplicationInfo* appInfo);
        Engine(const Engine&) = delete;
        ~Engine();

        Engine& operator=(const Engine&) = delete;

    private:
        const EngineInfo engineInfo{ .name = "PECS",
                                     .version = VK_MAKE_API_VERSION(0, 1, 0, 0) };
        
        Window * window;
};

}


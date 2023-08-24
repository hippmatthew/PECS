/*
 *  PECS - debug.hpp
 *  Author:     Matthew Hipp
 *  Created:    7/21/23
 *  Updated:    7/22/23
 */

#ifndef pecs_debug_hpp
#define pecs_debug_hpp

#ifndef VULKAN_HPP_NO_CONSTRUCTORS
    #define VULKAN_HPP_NO_CONSTRUCTORS
#endif

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_to_string.hpp>

#include <iostream>
#include <string>

namespace pecs
{
    
class DebugManager
{
    public:            
        DebugManager(bool enable = true) : enabled(enable) {};
        DebugManager(const DebugManager&) = delete;
            
        ~DebugManager() = default;

        DebugManager operator=(const DebugManager&) = delete;

        bool isEnabled() const;

        void message(const std::string s, bool err = false) const;
        void message(const vk::Result r) const;

    private:
        const bool enabled;
};

}

#endif /* pecs_debug_hpp */
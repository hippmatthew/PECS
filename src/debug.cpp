/*
 *  PECS - debug.cpp
 *  Author:     Matthew Hipp
 *  Created:    7/21/23
 *  Updated:    7/23/23
 */

#include "include/debug.hpp"

namespace pecs
{
    
    bool DebugManager::isEnabled() const
    { return enabled; }

    void DebugManager::message(const std::string s, bool err) const
    { 
        if (err)
            throw std::runtime_error("error: " + s + '\n');

        std::cout << s << '\n';
    }

    void DebugManager::message(const vk::Result r) const
    { 
        std::string s = vk::to_string(r);
        
        if (s.find("ERROR") != std::string::npos)
            throw std::runtime_error("error: " + s + '\n');
                
        std::cout << s << '\n';
    }

}
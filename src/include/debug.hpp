/*
*   PECS - debug.hpp
*   Author:     Matthew Hipp
*   Created:    6/29/23
*   Updated:    6/29/23
*/

#ifndef pecs_debug_hpp
#define pecs_debug_hpp

#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstring>

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace pecs
{

class DebugManager
{
    public:
        
        DebugManager(const DebugManager&) = delete;
        ~DebugManager();

        DebugManager& operator=(const DebugManager&) = delete;

        bool isEnabled() const;
        std::vector<const char *> getValidationLayers() const;
        
        void setupDebugMessenger() const;
        bool checkValidationLayerSupport() const;

        static DebugManager* getInstance();
        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                              vk::DebugUtilsMessageTypeFlagsEXT messageType,
                                                              const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                              void * pUserData);

    private:
        DebugManager();
        static DebugManager sInstance;

        vk::DebugUtilsMessengerEXT debugMessenger;
        
        #ifdef NDEBUG
            const bool debugMode = false;
        #else
            const bool debugMode = true;
        #endif

        std::vector<const char *> validationLayers = { "VK_LAYER_KHRONOS_validation" };
};

}

#endif /* pecs_debug_hpp */
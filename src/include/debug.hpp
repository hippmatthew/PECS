/*
 *  PECS - debug.hpp
 *  Author:     Matthew Hipp
 *  Created:    6/29/23
 *  Updated:    7/18/23
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
        
        void setupDebugMessenger(const vk::Instance& instance) const;
        bool checkValidationLayerSupport() const;

        static DebugManager* getInstance();

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

        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                              VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                              void * pUserData);
};

}

#endif /* pecs_debug_hpp */
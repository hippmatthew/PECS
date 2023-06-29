/*
*   PECS - debug.cpp
*   Author:     Matthew Hipp
*   Created:    6/29/23
*   Updated:    6/29/23
*/

#include "include/debug.hpp"

namespace pecs
{

    DebugManager DebugManager::sInstance;

    DebugManager::DebugManager() {}
    DebugManager::~DebugManager() {}

    bool DebugManager::isEnabled() const
    { return debugMode; }
    
    DebugManager* DebugManager::getInstance()
    { return &sInstance; }

    std::vector<const char *> DebugManager::getValidationLayers() const
    { return validationLayers; }

    void DebugManager::setupDebugMessenger() const
    {
        if (!debugMode) return;

        vk::DebugUtilsMessengerCreateInfoEXT{ .messageSeverity  = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose    |
                                                                  vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo       |
                                                                  vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning    |
                                                                  vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
                                              .messageType      = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral        |
                                                                  vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance    |
                                                                  vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
                                              .pfnUserCallback  = debugCallback,
                                              .pUserData        = nullptr}
    }
    
    bool DebugManager::checkValidationLayerSupport() const
    {
        unsigned int layerCount = 0;
        vk::enumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<vk::LayerProperties> availableLayers(layerCount);
        vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char * layerName : validationLayers)
        {
            for (const auto& layerProperties : availableLayers)
                if (strcmp(layerName, layerProperties.layerName) == 0)
                    return true;

            return false;
        }
            
    }

    VKAPI_ATTR vk::Bool32 VKAPI_CALL debugMessenger(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    vk::DebugUtilsMessageTypeFlagsEXT messageType,
                                                    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void * pUserData)
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
}
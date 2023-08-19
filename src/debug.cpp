/*
 *  PECS - debug.cpp
 *  Author:     Matthew Hipp
 *  Created:    6/29/23
 *  Updated:    7/18/23
 */

#include "include/debug.hpp"

namespace pecs
{  
   DebugManager * DebugManager::sInstance = nullptr;
   
   DebugManager::~DebugManager()
   {
        delete sInstance;
   }
   
   bool DebugManager::isEnabled()
    { return debugMode; }
    
    DebugManager * DebugManager::getInstance()
    { return sInstance; }

    void DebugManager::initialize()
    { 
        if (sInstance != nullptr) return;

        sInstance = new DebugManager;
    }
    
    std::vector<const char *> DebugManager::getValidationLayers() const
    { return validationLayers; }

    void DebugManager::setupDebugMessenger(const vk::Instance& instance) const
    {
        if (!debugMode) return;

        vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
        populateMessengerStruct(debugMessengerCreateInfo);

        auto dldi = vk::DispatchLoaderDynamic(instance, nullptr);
        auto messenger = instance.createDebugUtilsMessengerEXT(debugMessengerCreateInfo, nullptr, dldi);
    }
    
    bool DebugManager::checkValidationLayerSupport() const
    {
        unsigned int layerCount = 0;
        static_cast<void>(vk::enumerateInstanceLayerProperties(&layerCount, nullptr));

        std::vector<vk::LayerProperties> availableLayers(layerCount);
        static_cast<void>(vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

        for (const char * layerName : validationLayers)
        {
            for (const auto& layerProperties : availableLayers)
                if (strcmp(layerName, layerProperties.layerName) == 0)
                    return true;

            return false;
        }

        return false;
    }

    void DebugManager::populateMessengerStruct(vk::DebugUtilsMessengerCreateInfoEXT& info)
    {
        info.messageSeverity    = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose    |
                                  vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo       |
                                  vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning    |
                                  vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
        info.messageType        = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral        |
                                  vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance    |
                                  vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
        info.pfnUserCallback    = debugCallback;
        info.pUserData          = nullptr;
    }

    VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugManager::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                 VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                                 const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                                 void * pUserData)
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
}
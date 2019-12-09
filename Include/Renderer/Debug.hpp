//
// Created by Timothée Couble on 08/12/2019.
//

#ifndef DEBUG_HPP
# define DEBUG_HPP

#include <iostream>
#include <vector>

#include "Libraries.hpp"

namespace renderer {

    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

    class Debug {
    public:
        Debug() = default;

        void setUp(VkInstance &instance);
        void cleanUp(VkInstance &instance);

        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    private:
        void setupDebugMessenger(VkInstance &instance);

        static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                const VkAllocationCallbacks *pAllocator,
                VkDebugUtilsMessengerEXT *pDebugMessenger);
        static void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                VkDebugUtilsMessengerEXT debugMessenger,
                const VkAllocationCallbacks *pAllocator);
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData);

    private:
        VkDebugUtilsMessengerEXT _debugMessenger;
    };
}

#endif /* !DEBUG_HPP */
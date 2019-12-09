//
// Created by Timothée Couble on 08/12/2019.
//

#include "Debug.hpp"

void renderer::Debug::setUp(VkInstance &instance) {
    setupDebugMessenger(instance);
}

void renderer::Debug::cleanUp(VkInstance &instance) {
    if (enableValidationLayers)
        DestroyDebugUtilsMessengerEXT(instance, _debugMessenger, nullptr);
}

void renderer::Debug::setupDebugMessenger(VkInstance &instance) {
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS)
        throw std::runtime_error("failed to set up debug messenger!");

}

void renderer::Debug::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

VkResult renderer::Debug::CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                       const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                       const VkAllocationCallbacks *pAllocator,
                                                       VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void renderer::Debug::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                                    const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }

}

VkBool32 renderer::Debug::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                        void *pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

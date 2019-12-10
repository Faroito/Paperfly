//
// Created by Timothée Couble on 09/12/2019.
//

#ifndef UTILS_HPP
# define UTILS_HPP

#include <iostream>
#include <vector>
#include <optional>
#include <fstream>
#include <cstdint>
#include <cstdlib>

#include "Libraries.hpp"

namespace renderer {
    const int WIDTH = 1024;
    const int HEIGHT = 768;
    const int MAX_FRAMES_IN_FLIGHT = 2;

    const std::string MODEL_PATH = "../Resources/chalet.obj";
    const std::string TEXTURE_PATH = "../Resources/chalet.jpg";

    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };
    const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    std::vector<char> readFile(const std::string& filename);
    VkFormat findDepthFormat(VkPhysicalDevice &device);
    VkFormat findSupportedFormat(VkPhysicalDevice &device, const std::vector<VkFormat>& candidates,
                                 VkImageTiling tiling, VkFormatFeatureFlags features);
    uint32_t findMemoryType(VkPhysicalDevice &device, uint32_t typeFilter, VkMemoryPropertyFlags properties);
}

#endif /* !UTILS_HPP */

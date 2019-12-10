//
// Created by Timothée Couble on 09/12/2019.
//

#include "SwapChain.hpp"

void renderer::SwapChain::setUp(GLFWwindow* window, Surface &surface, Devices &devices) {
    createSwapChain(window, surface, devices);
    createImageViews(devices.get());
}

void renderer::SwapChain::createSwapChain(GLFWwindow* window, Surface &surface, Devices &devices) {
    SwapChainSupportDetails swapChainSupport = surface.querySwapChainSupport(devices.getPhysical());

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(window, swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface.get();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = surface.findQueueFamilies(devices.getPhysical());
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(devices.get(), &createInfo, nullptr, &_swapChain) != VK_SUCCESS)
        throw std::runtime_error("failed to create swap chain!");

    vkGetSwapchainImagesKHR(devices.get(), _swapChain, &imageCount, nullptr);
    _swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(devices.get(), _swapChain, &imageCount, _swapChainImages.data());

    _swapChainExtent = extent;
    _swapChainImageFormat = surfaceFormat.format;
}

void renderer::SwapChain::createImageViews(VkDevice &device) {
    _swapChainImageViews.resize(_swapChainImages.size());

    for (uint32_t i = 0; i < _swapChainImages.size(); i++)
        _swapChainImageViews[i] = BufferManip::createImageView(device, _swapChainImages[i],
                                                  _swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

}

void renderer::SwapChain::cleanUp(VkDevice &device) {
    for (auto &imageView : _swapChainImageViews)
        vkDestroyImageView(device, imageView, nullptr);
    vkDestroySwapchainKHR(device, _swapChain, nullptr);
}

VkSwapchainKHR &renderer::SwapChain::get() {
    return _swapChain;
}

VkFormat &renderer::SwapChain::getImageFormat() {
    return _swapChainImageFormat;
}

VkImageView &renderer::SwapChain::getImageView(size_t i) {
    return _swapChainImageViews[i];
}

VkExtent2D &renderer::SwapChain::getExtent() {
    return _swapChainExtent;
}

size_t renderer::SwapChain::size() {
    return _swapChainImages.size();
}

float renderer::SwapChain::ratio() {
    return _swapChainExtent.width / (float) _swapChainExtent.height;
}

VkSurfaceFormatKHR
renderer::SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;
        }

        return availableFormats[0];
}

VkPresentModeKHR
renderer::SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes)
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D renderer::SwapChain::chooseSwapExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
        };

        actualExtent.width = std::max(capabilities.minImageExtent.width,
                                      std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height,
                                       std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

//
// Created by Timothée Couble on 09/12/2019.
//

#ifndef SWAP_CHAIN_HPP
# define SWAP_CHAIN_HPP

#include <vector>

#include "BufferManip.hpp"
#include "Devices.hpp"
#include "Libraries.hpp"
#include "Surface.hpp"
#include "Utils.hpp"

namespace renderer {

    /**
     * The swap chain is essentially a queue of images
     * that are waiting to be presented to the screen.
     */
    class SwapChain {
    public:
        SwapChain() = default;

        void setUp(GLFWwindow* window, Surface &surface, Devices &devices);
        void cleanUp(VkDevice &device);

        VkSwapchainKHR &get();
        VkFormat &getImageFormat();
        VkImageView &getImageView(size_t i);
        VkExtent2D &getExtent();
        size_t size();
        float ratio();


    private:
        void createSwapChain(GLFWwindow* window, Surface &surface, Devices &devices);
        void createImageViews(VkDevice &device);

        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        static VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);

    private:
        VkSwapchainKHR _swapChain = nullptr;
        VkFormat _swapChainImageFormat = {};
        std::vector<VkImage> _swapChainImages;
        std::vector<VkImageView> _swapChainImageViews;
        VkExtent2D _swapChainExtent = {};

    };
}

#endif /* !SWAP_CHAIN_HPP */

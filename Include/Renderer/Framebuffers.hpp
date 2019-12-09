//
// Created by Timothée Couble on 09/12/2019.
//

#ifndef FRAMEBUFFERS_HPP
# define FRAMEBUFFERS_HPP

#include <vector>
#include <array>

#include "Devices.hpp"
#include "Libraries.hpp"
#include "SwapChain.hpp"
#include "Utils.hpp"

namespace renderer {

    /**
     * Framebuffer for all of the images in the swap chain
     * and use the one that corresponds to the retrieved image
     * at drawing time
     */
    class Framebuffers {
    public:
        Framebuffers() = default;

        void setUp(VkDevice &device, SwapChain &swapChain, VkRenderPass &renderPass, VkImageView &depthImageView);
        void cleanUp(VkDevice &device);

        VkFramebuffer &operator[](size_t i);
        size_t size() const;

    private:
        std::vector<VkFramebuffer> _swapChainFramebuffers;

    };

}

#endif /* !FRAMEBUFFERS_HPP */

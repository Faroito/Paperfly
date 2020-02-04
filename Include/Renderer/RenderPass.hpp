//
// Created by Timothée Couble on 03/02/2020.
//

#ifndef RENDERPASS_HPP
# define RENDERPASS_HPP

#include <array>

#include "Devices.hpp"
#include "Libraries.hpp"
#include "SwapChain.hpp"
#include "Utils.hpp"

namespace renderer {

    class RenderPass {
    public:
        RenderPass() = default;

        void setUp(Devices &device, SwapChain &swapChain);
        void cleanUp(VkDevice &device);

        VkRenderPass &get();

    private:
        VkRenderPass _renderPass;
    };

}

#endif /* !RENDERPASS_HPP */

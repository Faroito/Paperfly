//
// Created by Timothée Couble on 09/12/2019.
//

#ifndef COMMAND_BUFFERS_HPP
# define COMMAND_BUFFERS_HPP

#include <vector>
#include <array>

#include "Framebuffers.hpp"
#include "GraphicsPipeline.hpp"
#include "Libraries.hpp"
#include "SwapChain.hpp"
#include "Model.hpp"
#include "Texture.hpp"
#include "UniformBuffers.hpp"
#include "Utils.hpp"

namespace renderer {

    /**
     * Record all of the operations to perform in command buffer objects.
     * Vulkan execute the commands in the main loop.
     */
    class CommandBuffers {
    public:
        CommandBuffers() = default;

        void setUp(VkDevice &device, SwapChain &swapChain, GraphicsPipeline &pipeline, Framebuffers &framebuffers,
                   VkCommandPool &pool, Model &model, Model &model2);
        void cleanUp(VkDevice &device, VkCommandPool &pool);

        VkCommandBuffer &operator[](size_t i);
        size_t size() const;

    private:
        std::vector<VkCommandBuffer> _commandBuffers;
    };

}

#endif /* !COMMAND_BUFFERS_HPP */

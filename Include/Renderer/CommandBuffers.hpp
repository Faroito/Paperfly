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

        void setUp(VkDevice &device, SwapChain &swapChain, GraphicsPipeline &pipeline,
                   Framebuffers &framebuffers, VkCommandPool &pool, Texture &texture,
                   VkBuffer &vertexBuffer, VkBuffer &indexBuffer, size_t size, UniformBuffers &uniforms);
        void cleanUp(VkDevice &device, VkCommandPool &pool);

        VkCommandBuffer &operator[](size_t i);
        size_t size() const;

    private:
        void createDescriptorPool(VkDevice &device, size_t size);
        void createDescriptorSets(VkDevice &device, size_t size, VkDescriptorSetLayout &layout,
                                  Texture &texture, UniformBuffers &uniforms);
        void createCommandBuffers(VkDevice &device, VkExtent2D &swapChainExtent,
                                  renderer::GraphicsPipeline &pipeline, renderer::Framebuffers &framebuffers,
                                  VkCommandPool &pool, VkBuffer &vertexBuffer, VkBuffer &indexBuffer, size_t size);

    private:
        VkDescriptorPool _descriptorPool = nullptr;
        std::vector<VkDescriptorSet> _descriptorSets;
        std::vector<VkCommandBuffer> _commandBuffers;
    };

}

#endif /* !COMMAND_BUFFERS_HPP */

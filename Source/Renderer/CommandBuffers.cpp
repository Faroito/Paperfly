//
// Created by Timothée Couble on 09/12/2019.
//

#include "CommandBuffers.hpp"

void renderer::CommandBuffers::setUp(VkDevice &device, SwapChain &swapChain, GraphicsPipeline &pipeline,
                                     Framebuffers &framebuffers, VkCommandPool &pool, Model &model, Model &model2) {
    _commandBuffers.resize(framebuffers.size());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) _commandBuffers.size();

    if (vkAllocateCommandBuffers(device, &allocInfo, _commandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate command buffers!");

    for (size_t i = 0; i < _commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(_commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = pipeline.getRenderPass();
        renderPassInfo.framebuffer = framebuffers[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain.getExtent();

        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = {0.07f, 0.17f, 0.24f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get());

        /**
         * Start
         */

        VkBuffer vertexBuffers[] = {model.getVertexBuffer()};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(_commandBuffers[i], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(_commandBuffers[i], model.getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipeline.getLayout(), 0, 1, &model.getDescriptorSet(i), 0, nullptr);
        vkCmdDrawIndexed(_commandBuffers[i], model.getIndicesSize(), 1, 0, 0, 0);


        VkBuffer vertexBuffers2[] = {model2.getVertexBuffer()};

        vkCmdBindVertexBuffers(_commandBuffers[i], 0, 1, vertexBuffers2, offsets);
        vkCmdBindIndexBuffer(_commandBuffers[i], model2.getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipeline.getLayout(), 0, 1, &model2.getDescriptorSet(i), 0, nullptr);
        vkCmdDrawIndexed(_commandBuffers[i], model2.getIndicesSize(), 1, 0, 0, 0);

        /**
        * End
        */

        vkCmdEndRenderPass(_commandBuffers[i]);

        if (vkEndCommandBuffer(_commandBuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to record command buffer!");
    }
}

void renderer::CommandBuffers::cleanUp(VkDevice &device, VkCommandPool &pool) {
    vkFreeCommandBuffers(device, pool,
                         static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
}

VkCommandBuffer &renderer::CommandBuffers::operator[](size_t i) {
    return _commandBuffers[i];
}

size_t renderer::CommandBuffers::size() const {
    return _commandBuffers.size();
}

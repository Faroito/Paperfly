//
// Created by Timothée Couble on 10/12/2019.
//

#include "CommandPool.hpp"

void renderer::CommandPool::setUp(VkDevice &device, renderer::QueueFamilyIndices queueFamilyIndices) {
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = 0;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create command pool!");
}

void renderer::CommandPool::cleanUp(VkDevice &device) {
    vkDestroyCommandPool(device, _commandPool, nullptr);

}

VkCommandPool &renderer::CommandPool::get() {
    return _commandPool;
}

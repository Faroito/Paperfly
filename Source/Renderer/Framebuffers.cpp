//
// Created by Timothée Couble on 09/12/2019.
//

#include "Framebuffers.hpp"

void renderer::Framebuffers::setUp(VkDevice &device, renderer::SwapChain &swapChain,
        VkRenderPass &renderPass, VkImageView &depthImageView) {
    _swapChainFramebuffers.resize(swapChain.size());

    for (size_t i = 0; i < swapChain.size(); i++) {
        std::array<VkImageView, 2> attachments = {
                swapChain.getImageView(i),
                depthImageView
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChain.getExtent().width;
        framebufferInfo.height = swapChain.getExtent().height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create framebuffer!");
    }
}

void renderer::Framebuffers::cleanUp(VkDevice &device) {
    for (auto &framebuffer : _swapChainFramebuffers)
        vkDestroyFramebuffer(device, framebuffer, nullptr);
}

VkFramebuffer &renderer::Framebuffers::operator[](size_t i) {
    return _swapChainFramebuffers[i];
}

size_t renderer::Framebuffers::size() const {
    return _swapChainFramebuffers.size();
}

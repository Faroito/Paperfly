//
// Created by Timothée Couble on 09/12/2019.
//

#ifndef GRAPHICS_PIPELINE_HPP
# define GRAPHICS_PIPELINE_HPP

#include <vector>
#include <array>

#include "Devices.hpp"
#include "Libraries.hpp"
#include "SwapChain.hpp"
#include "Utils.hpp"
#include "Vertex.hpp"

namespace renderer {

    /**
     * The graphics pipeline is the sequence of operations
     * that take the vertices and textures of your meshes
     * all the way to the pixels in the render targets.
     */
    class GraphicsPipeline {
    public:
        GraphicsPipeline() = default;

        void setUp(Devices &device, SwapChain &swapChain);
        void cleanUp(VkDevice &device);

        VkPipeline &get();
        VkRenderPass &getRenderPass();
        VkPipelineLayout &getLayout();
        VkDescriptorSetLayout &getDescriptorSetLayout();

    private:
        void createRenderPass(Devices &device, SwapChain &swapChain);
        void createDescriptorSetLayout(VkDevice &device);
        void createGraphicsPipeline(VkDevice &device, SwapChain &swapChain);

        static VkShaderModule createShaderModule(VkDevice &device, const std::vector<char>& code);

    private:
        VkRenderPass _renderPass;
        VkPipeline _graphicsPipeline;
        VkPipelineLayout _pipelineLayout;
        VkDescriptorSetLayout _descriptorSetLayout;

    };

}

#endif /* !GRAPHICS_PIPELINE_HPP */

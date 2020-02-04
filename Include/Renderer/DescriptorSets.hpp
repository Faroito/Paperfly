//
// Created by Timothée Couble on 04/02/2020.
//

#ifndef DESCRIPTOR_SETS_HPP
# define DESCRIPTOR_SETS_HPP

#include <vector>

#include "Devices.hpp"
#include "Libraries.hpp"
#include "SwapChain.hpp"
#include "Texture.hpp"
#include "UniformBuffers.hpp"

namespace renderer {

    /**
     *
     */
    class DescriptorSets {
    public:
        DescriptorSets() = default;

        void setUp(VkDevice &device, SwapChain &swapChain, VkDescriptorSetLayout &layout,
                   Texture &texture, UniformBuffers &uniforms);
        void cleanUp(VkDevice &device);

        VkDescriptorSet &operator[](size_t i);

    private:
        void createDescriptorPool(VkDevice &device, size_t size);
        void createDescriptorSets(VkDevice &device, size_t size, VkDescriptorSetLayout &layout,
                                  Texture &texture, UniformBuffers &uniforms);

    private:
        VkDescriptorPool _descriptorPool = nullptr;
        std::vector<VkDescriptorSet> _descriptorSets;
    };

}

#endif /* !DESCRIPTOR_SETS_HPP */

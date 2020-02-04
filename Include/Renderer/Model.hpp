//
// Created by Timothée Couble on 09/12/2019.
//

#ifndef MODEL_HPP
# define MODEL_HPP

#include <chrono>
#include <vector>

#include "Camera.hpp"
#include "Framebuffers.hpp"
#include "GraphicsPipeline.hpp"
#include "DescriptorSets.hpp"
#include "Devices.hpp"
#include "Libraries.hpp"
#include "Texture.hpp"
#include "UniformBuffers.hpp"
#include "Utils.hpp"

namespace renderer {

    /**
     */
    class Model {
    public:
        explicit Model(ModelType type, ModelColor color);

        void setUp(Devices &devices, SwapChain &swapChain, GraphicsPipeline &pipeline,
                Framebuffers &framebuffers, VkCommandPool &pool, TextureMap_t &textures);
        void cleanUp(VkDevice &device);

        void setPosition(glm::vec3 position);
        void setCamera(scene::Camera_ptr_t &_camera, float ratio);
        void updateUniformBuffer(VkDevice &device, uint32_t currentImage);

        ModelType getModelType() const;
        VkDescriptorSet &getDescriptorSet(size_t i);

    private:
        const ModelType _type;
        const ModelColor _color;

        UniformBufferObject _ubo = {};
        UniformBuffers _uniforms;
        DescriptorSets _descriptorSets;

        glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 0.0f);
    };

    typedef std::vector<Model> Models_t;
}

#endif /* !MODEL_HPP */

//
// Created by Timothée Couble on 09/12/2019.
//

#ifndef MODEL_HPP
# define MODEL_HPP

#include <vector>
#include <memory>
#include <numeric>

#include "Camera.hpp"
#include "Framebuffers.hpp"
#include "GraphicsPipeline.hpp"
#include "DescriptorSets.hpp"
#include "Devices.hpp"
#include "Mesh.hpp"
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
        virtual ~Model() = default;

        void setUp(Devices &devices, SwapChain &swapChain, GraphicsPipeline &pipeline,
                Framebuffers &framebuffers, VkCommandPool &pool, MeshMap_t &meshes);
        void cleanUp(VkDevice &device);

        void setPosition(glm::vec3 position);
        void setOrientation(glm::vec3 orientation);
        void setCamera(scene::Camera_ptr_t &_camera, float ratio);

        virtual void update(std::vector<std::unique_ptr<Model>> &models);
        virtual void updateUniformBuffer(VkDevice &device, uint32_t currentImage);
        virtual bool willCollide(glm::vec3 position);

        size_t getId() const;
        glm::vec3 getPosition() const;
        glm::vec3 getVelocity() const;
        ModelType getModelType() const;
        VkDescriptorSet &getDescriptorSet(size_t i);

    protected:
        glm::vec3 _velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 _orientation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 _offset = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 _scale = glm::vec3(1.0f, 1.0f, 1.0f);

    private:
        const size_t _id;
        static size_t _idGen;
        const ModelType _type;
        const ModelColor _color;

        UniformBufferObject _ubo = {};
        UniformBuffers _uniforms;
        DescriptorSets _descriptorSets;
    };

    typedef std::vector<std::unique_ptr<Model>> Models_t;
}


#endif /* !MODEL_HPP */

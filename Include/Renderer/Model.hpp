//
// Created by Timothée Couble on 09/12/2019.
//

#ifndef MODEL_HPP
# define MODEL_HPP

#include <chrono>
#include <vector>
#include <unordered_map>

#include "BufferManip.hpp"
#include "Camera.hpp"
#include "Framebuffers.hpp"
#include "GraphicsPipeline.hpp"
#include "DescriptorSets.hpp"
#include "Devices.hpp"
#include "Libraries.hpp"
#include "Texture.hpp"
#include "UniformBuffers.hpp"
#include "Utils.hpp"
#include "Vertex.hpp"

namespace renderer {

    /**
     * Load the vertices and indices from an obj model file.
     * Load also the texture file.
     */
    class Model {
    public:
        explicit Model(const std::string &textureName = "pp_orange_texture.png",
                const std::string &modelName = "paper_plane.obj");

        void setUp(Devices &devices, VkCommandPool &pool);
        void cleanUp(VkDevice &device);

        void setUpSwapChain(Devices &devices, SwapChain &swapChain, GraphicsPipeline &pipeline,
                Framebuffers &framebuffers, VkCommandPool &pool);
        void cleanUpSwapChain(VkDevice &device);

        void setPosition(glm::vec3 position);
        void setCamera(scene::Camera_ptr_t &_camera, float ratio);
        void updateUniformBuffer(VkDevice &device, uint32_t currentImage);

        VkBuffer &getVertexBuffer();
        VkBuffer &getIndexBuffer();
        uint32_t getIndicesSize();
        VkDescriptorSet &getDescriptorSet(size_t i);

    private:
        void loadModel();
        void createVertexBuffer(Devices &devices, VkCommandPool &pool);
        void createIndexBuffer(Devices &devices, VkCommandPool &pool);

    private:
        std::vector<Vertex> _vertices;
        std::vector<uint32_t> _indices;

        VkBuffer _vertexBuffer;
        VkDeviceMemory _vertexBufferMemory;
        VkBuffer _indexBuffer;
        VkDeviceMemory _indexBufferMemory;

        Texture _texture;

        UniformBufferObject _ubo = {};
        UniformBuffers _uniforms;
        DescriptorSets _descriptorSets;

        glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 0.0f);

        const std::string _textureName;
        const std::string _modelName;
    };
}

#endif /* !MODEL_HPP */

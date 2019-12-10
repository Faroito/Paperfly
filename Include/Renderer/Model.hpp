//
// Created by Timothée Couble on 09/12/2019.
//

#ifndef MODEL_HPP
# define MODEL_HPP

#include <vector>
#include <unordered_map>

#include "BufferManip.hpp"
#include "Devices.hpp"
#include "Libraries.hpp"
#include "Texture.hpp"
#include "Utils.hpp"
#include "Vertex.hpp"

namespace renderer {

    /**
     * Load the vertices and indices from an obj model file.
     * Load also the texture file.
     */
    class Model {
    public:
        Model() = default;

        void setUp(Devices &devices, VkCommandPool &pool);
        void cleanUp(VkDevice &device);

        VkBuffer &getVertexBuffer();
        VkBuffer &getIndexBuffer();
        Texture &getTexture();
        size_t size() const;

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

    };
}

#endif /* !MODEL_HPP */

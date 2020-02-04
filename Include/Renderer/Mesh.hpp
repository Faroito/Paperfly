//
// Created by Timothée Couble on 04/02/2020.
//

#ifndef MESH_HPP
# define MESH_HPP

#include <vector>
#include <unordered_map>

#include "BufferManip.hpp"
#include "Devices.hpp"
#include "Libraries.hpp"
#include "Utils.hpp"
#include "Vertex.hpp"

namespace renderer {

    /**
     * Load the vertices and indices from an obj model file.
     */
    class Mesh {
    public:
        explicit Mesh(ModelType type);

        void setUp(Devices &devices, VkCommandPool &pool);
        void cleanUp(VkDevice &device);

        VkBuffer &getVertexBuffer();
        VkBuffer &getIndexBuffer();
        uint32_t getIndicesSize() const;

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

        const ModelType _type;
        const std::unordered_map<ModelType, std::string> _modelFile = {
                {ModelType::PAPER_PLANE, "paper_plane.obj"}
        };

    };

    typedef std::unordered_map<ModelType, Mesh> MeshMap_t;
}

#endif /* !MESH_HPP */

//
// Created by Timothée Couble on 04/02/2020.
//

#include <tiny_obj_loader.h>

#include "Mesh.hpp"

renderer::Mesh::Mesh(const ModelType type) : _type(type) {}

void renderer::Mesh::setUp(renderer::Devices &devices, VkCommandPool &pool) {
    loadModel();
    createVertexBuffer(devices, pool);
    createIndexBuffer(devices, pool);
}


void renderer::Mesh::loadModel() {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    std::string path = PATH + _modelFile.at(_type);

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
        throw std::runtime_error(warn + err);

    std::unordered_map<Vertex, uint32_t> uniqueVertices = {};
    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            Vertex vertex = {};
            vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
            };
            vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };
            vertex.color = {1.0f, 1.0f, 1.0f};

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(_vertices.size());
                _vertices.push_back(vertex);
            }
            _indices.push_back(uniqueVertices[vertex]);
        }
    }

}

void renderer::Mesh::createVertexBuffer(renderer::Devices &devices, VkCommandPool &pool) {
    VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    BufferManip::createBuffer(devices, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(devices.get(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, _vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(devices.get(), stagingBufferMemory);

    BufferManip::createBuffer(devices, bufferSize,
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer, _vertexBufferMemory);

    BufferManip::copyBuffer(devices, pool, stagingBuffer, _vertexBuffer, bufferSize);

    vkDestroyBuffer(devices.get(), stagingBuffer, nullptr);
    vkFreeMemory(devices.get(), stagingBufferMemory, nullptr);
}

void renderer::Mesh::createIndexBuffer(renderer::Devices &devices, VkCommandPool &pool) {
    VkDeviceSize bufferSize = sizeof(_indices[0]) * _indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    BufferManip::createBuffer(devices, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(devices.get(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, _indices.data(), (size_t) bufferSize);
    vkUnmapMemory(devices.get(), stagingBufferMemory);

    BufferManip::createBuffer(devices, bufferSize,
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);

    BufferManip::copyBuffer(devices, pool, stagingBuffer, _indexBuffer, bufferSize);

    vkDestroyBuffer(devices.get(), stagingBuffer, nullptr);
    vkFreeMemory(devices.get(), stagingBufferMemory, nullptr);
}

void renderer::Mesh::cleanUp(VkDevice &device) {
    vkDestroyBuffer(device, _vertexBuffer, nullptr);
    vkFreeMemory(device, _vertexBufferMemory, nullptr);
    vkDestroyBuffer(device, _indexBuffer, nullptr);
    vkFreeMemory(device, _indexBufferMemory, nullptr);
}

VkBuffer &renderer::Mesh::getVertexBuffer() {
    return _vertexBuffer;
}

VkBuffer &renderer::Mesh::getIndexBuffer() {
    return _indexBuffer;
}

uint32_t renderer::Mesh::getIndicesSize() const {
    return _indices.size();
}
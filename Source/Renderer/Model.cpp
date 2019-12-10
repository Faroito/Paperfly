//
// Created by Timothée Couble on 09/12/2019.
//

#include <tiny_obj_loader.h>

#include "Model.hpp"

void renderer::Model::setUp(Devices &devices, VkCommandPool &pool) {
    _texture.setUp(devices, pool);
    loadModel();
    createVertexBuffer(devices, pool);
    createIndexBuffer(devices, pool);
}

void renderer::Model::loadModel() {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str()))
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

void renderer::Model::createVertexBuffer(Devices &devices, VkCommandPool &pool) {
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

void renderer::Model::createIndexBuffer(Devices &devices, VkCommandPool &pool) {
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

void renderer::Model::cleanUp(VkDevice &device) {
    vkDestroyBuffer(device, _vertexBuffer, nullptr);
    vkFreeMemory(device, _vertexBufferMemory, nullptr);
    vkDestroyBuffer(device, _indexBuffer, nullptr);
    vkFreeMemory(device, _indexBufferMemory, nullptr);
    _texture.cleanUp(device);
}

void renderer::Model::setUpSwapChain(Devices &devices, SwapChain &swapChain, GraphicsPipeline &pipeline,
        Framebuffers &framebuffers, VkCommandPool &pool) {
    _uniforms.setUp(devices, swapChain.size());
    _commandBuffers.setUp(devices.get(), swapChain, pipeline, framebuffers, pool,
            _texture, _vertexBuffer,_indexBuffer, _indices.size(), _uniforms);
}

void renderer::Model::cleanUpSwapChain(VkDevice &device, VkCommandPool &pool) {
    _commandBuffers.cleanUp(device, pool);
    _uniforms.cleanUp(device);
}

void renderer::Model::setCamera(scene::Camera_ptr_t &_camera, float ratio) {
    _ubo.view = _camera->getViewMatrix();
    _ubo.proj = _camera->getProjectionMatrix(ratio);
}

void renderer::Model::updateUniformBuffer(VkDevice &device, uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    _ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(25.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    void* data;
    vkMapMemory(device, _uniforms.getMemory(currentImage), 0, sizeof(_ubo), 0, &data);
    memcpy(data, &_ubo, sizeof(_ubo));
    vkUnmapMemory(device, _uniforms.getMemory(currentImage));
}

VkCommandBuffer &renderer::Model::getCommandBuffers(uint32_t i) {
    return _commandBuffers[i];
}

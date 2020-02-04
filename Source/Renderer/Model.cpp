//
// Created by Timothée Couble on 09/12/2019.
//

#include "Model.hpp"

renderer::Model::Model(const renderer::ModelType type, const renderer::ModelColor color) : _type(type), _color(color) {}

void
renderer::Model::setUp(renderer::Devices &devices, renderer::SwapChain &swapChain, renderer::GraphicsPipeline &pipeline,
                       renderer::Framebuffers &framebuffers, VkCommandPool &pool, TextureMap_t &textures) {
    _uniforms.setUp(devices, swapChain.size());
    _descriptorSets.setUp(devices.get(), swapChain, pipeline.getDescriptorSetLayout(), textures.at(_color), _uniforms);
}

void renderer::Model::cleanUp(VkDevice &device) {
    _descriptorSets.cleanUp(device);
    _uniforms.cleanUp(device);
}

void renderer::Model::setPosition(glm::vec3 position) {
    _pos = position;
}

void renderer::Model::setCamera(scene::Camera_ptr_t &_camera, float ratio) {
    _ubo.view = _camera->getViewMatrix();
    _ubo.proj = _camera->getProjectionMatrix(ratio);
}

void renderer::Model::updateUniformBuffer(VkDevice &device, uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    glm::mat4 translate = glm::translate(glm::mat4(1.0f), _pos);
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), time * glm::radians(25.0f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    _ubo.model = translate * rotate * scale;

    void* data;
    vkMapMemory(device, _uniforms.getMemory(currentImage), 0, sizeof(_ubo), 0, &data);
    memcpy(data, &_ubo, sizeof(_ubo));
    vkUnmapMemory(device, _uniforms.getMemory(currentImage));
}

renderer::ModelType renderer::Model::getModelType() const {
    return _type;
}

VkDescriptorSet &renderer::Model::getDescriptorSet(size_t i) {
    return _descriptorSets[i];
}


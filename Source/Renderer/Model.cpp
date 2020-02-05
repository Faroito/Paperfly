//
// Created by Timothée Couble on 09/12/2019.
//

#include "Model.hpp"

size_t renderer::Model::_idGen = 0;

renderer::Model::Model(const renderer::ModelType type, const renderer::ModelColor color)
    : _type(type), _color(color), _id(_idGen++) {}

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
    _position = position;
}

void renderer::Model::setOrientation(glm::vec3 orientation) {
    _orientation = orientation;
}

void renderer::Model::setCamera(scene::Camera_ptr_t &_camera, float ratio) {
    _ubo.view = _camera->getViewMatrix();
    _ubo.proj = _camera->getProjectionMatrix(ratio);
}

void renderer::Model::update(std::vector<std::unique_ptr<Model>> &models) {}

void renderer::Model::updateUniformBuffer(VkDevice &device, uint32_t currentImage) {
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), _position - _offset);
    glm::mat4 rotate = glm::translate(glm::mat4(1.0f), _offset);
    rotate = glm::rotate(rotate, glm::radians(_orientation.x), glm::vec3(1.f, 0.f, 0.f));
    rotate = glm::rotate(rotate, glm::radians(_orientation.y), glm::vec3(0.f, 1.f, 0.f));
    rotate = glm::rotate(rotate, glm::radians(_orientation.z), glm::vec3(0.f, 0.f, 1.f));
    rotate = glm::translate(rotate, -_offset);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    _ubo.model = translate * rotate * scale;

    void* data;
    vkMapMemory(device, _uniforms.getMemory(currentImage), 0, sizeof(_ubo), 0, &data);
    memcpy(data, &_ubo, sizeof(_ubo));
    vkUnmapMemory(device, _uniforms.getMemory(currentImage));
}

size_t renderer::Model::getId() const {
    return _id;
}

glm::vec3 renderer::Model::getPosition() const {
    return _position;
}

glm::vec3 renderer::Model::getVelocity() const {
    return _velocity;
}

renderer::ModelType renderer::Model::getModelType() const {
    return _type;
}

VkDescriptorSet &renderer::Model::getDescriptorSet(size_t i) {
    return _descriptorSets[i];
}


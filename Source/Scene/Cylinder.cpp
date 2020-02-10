//
// Created by Timothée Couble on 05/02/2020.
//

#include "Cylinder.hpp"

scene::Cylinder::Cylinder(renderer::ModelColor color) : renderer::Model(renderer::CYLINDER, color) {
    _scale = glm::vec3(0.5f, 0.8f, 0.5f);
    _size *= _scale;
}

void scene::Cylinder::update(renderer::Models_t &models) {}

void scene::Cylinder::updateUniformBuffer(VkDevice &device, uint32_t currentImage) {
    Model::updateUniformBuffer(device, currentImage);
}

bool scene::Cylinder::willCollide(glm::vec3 position) {
    auto bottomPosition = _position - _size / glm::vec3(2) - glm::vec3(_separationOffset);
    auto topPosition = _position + _size / glm::vec3(2) + glm::vec3(_separationOffset);

    return ((position.x < topPosition.x || position.x > bottomPosition.x) &&
            (position.y < topPosition.y || position.y > bottomPosition.y) &&
            (position.z < topPosition.z || position.z > bottomPosition.z));
}

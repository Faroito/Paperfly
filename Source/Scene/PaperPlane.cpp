//
// Created by Timothée Couble on 04/02/2020.
//

#include "PaperPlane.hpp"

scene::PaperPlane::PaperPlane(renderer::ModelColor color) : renderer::Model(renderer::PAPER_PLANE, color) {
    _velocity = glm::vec3(0.0f, 0.0f, 0.1f);
}

void scene::PaperPlane::update(renderer::Models_t &models) {
    // Calculate first rule: cohesion
    auto findCenter = [this](glm::vec3 center, std::unique_ptr<renderer::Model> &model) {
        if (this->getId() == model->getId())
            return center;
        else if (this->getId() == 0 && model->getId() == 1)
            return model->getPosition();
        return center + model->getPosition();
    };
    glm::vec3 center = std::accumulate(std::next(models.begin()), models.end(),
            models.front()->getPosition(), findCenter);
    center /= models.size() - 1;
    center = (center - _position) / glm::vec3(100);

    // Calculate second rule: separation
    glm::vec3 separation = glm::vec3(0.0f, 0.0f, 0.0f);
    for (auto &model : models) {
        if (this->getId() != model->getId()) {
            auto diff = model->getPosition() - _position;
            if (glm::length(diff) < _separationMin)
                separation -= diff;
        }
    }

    // Calculate third rule: alignment
    auto findVelocity = [this](glm::vec3 velocity, std::unique_ptr<renderer::Model> &model) {
        if (this->getId() == model->getId())
            return velocity;
        else if (this->getId() == 0 && model->getId() == 1)
            return model->getVelocity();
        return velocity + model->getVelocity();
    };
    glm::vec3 velocity = std::accumulate(std::next(models.begin()), models.end(),
                                       models.front()->getVelocity(), findVelocity);
    velocity /= (models.size() - 1);

    std::cout << velocity << std::endl;

    _velocity = center + separation + velocity + boundaries();

    // Limit Speed
    if (glm::length(_velocity) > _maxSpeed)
        _velocity = (_velocity / glm::length(_velocity)) * glm::vec3(_maxSpeed);

    _position += _velocity;
}

glm::vec3 scene::PaperPlane::boundaries() {
    glm::vec3 bound = glm::vec3(0.0f, 0.0f, 0.0f);
    if (_position.x < -10)
        bound.x = 1;
    else if (_position.x > 20)
        bound.x = -1;
    if (_position.y < -10)
        bound.y = 1;
    else if (_position.y > 20)
        bound.y = -1;
    if (_position.z < 0)
        bound.z = 1;
    else if (_position.z > 20)
        bound.z = -1;
    return bound;
}

void scene::PaperPlane::updateUniformBuffer (VkDevice &device, uint32_t currentImage) {
    glm::vec3 left_vector = glm::normalize(_velocity);

    double pitch = 0;
    if (left_vector.y < 0)
        pitch = glm::asin(left_vector.y) * (180 / M_PI);
    else
        pitch = -glm::asin(left_vector.y) * (180 / M_PI);
    double yaw = glm::atan(left_vector.x, left_vector.z) * (180 / M_PI);

    _orientation = glm::vec3(pitch, yaw - 180.0f, 0.0f);

    Model::updateUniformBuffer(device, currentImage);
}

std::ostream &operator<<(std::ostream &stream, const glm::vec3 &vec) {
    stream << vec.x << " " << vec.y << " " << vec.z;
    return stream;
}

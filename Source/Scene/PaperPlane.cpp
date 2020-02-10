//
// Created by Timothée Couble on 04/02/2020.
//

#include "PaperPlane.hpp"

scene::PaperPlane::PaperPlane(renderer::ModelColor color) : renderer::Model(renderer::PAPER_PLANE, color) {
    _velocity = glm::vec3(0.0f, 0.0f, 0.1f);
    _offset = glm::vec3(0.0f, 0.5f, 0.0f);
    _scale = glm::vec3(0.1f);
}

void scene::PaperPlane::update(renderer::Models_t &models) {
    size_t size = std::count_if(models.begin(), models.end(),
            [](std::unique_ptr<renderer::Model> &model){return model->getModelType() == renderer::PAPER_PLANE;});

    auto collision = this->collision(models) * glm::vec3(100);
    if (collision == glm::vec3(0.0f)) {
        _velocity += center(models, size) + separation(models) + alignment(models, size) + boundaries();
        //std::cout << "hello" << std::endl;
    } else {
        _velocity += collision + separation(models) + boundaries();
    std::cout << collision << std::endl;
    }
    // Limit Speed
    if (glm::length(_velocity) > _maxSpeed)
        _velocity = (_velocity / glm::length(_velocity)) * glm::vec3(_maxSpeed);

    _position += _velocity;
}

glm::vec3 scene::PaperPlane::center(renderer::Models_t &models, size_t &size) {
    auto findCenter = [this](glm::vec3 center, std::unique_ptr<renderer::Model> &model) {
        if (this->getId() == model->getId() || model->getModelType() != renderer::PAPER_PLANE)
            return center;
        else if (this->getId() == 0 && model->getId() == 1)
            return model->getPosition();
        return center + model->getPosition();
    };
    glm::vec3 center = std::accumulate(std::next(models.begin()), models.end(),
                                       models.front()->getPosition(), findCenter);
    center /= size;
    center = (center - _position) / glm::vec3(100);
    return center;
}

glm::vec3 scene::PaperPlane::separation(renderer::Models_t &models) {
    glm::vec3 separation = glm::vec3(0.0f, 0.0f, 0.0f);
    for (auto &model : models)
        if (this->getId() != model->getId() && model->getModelType() == renderer::PAPER_PLANE)
            if (willCollide(model->getPosition()))
                separation -= model->getPosition() - _position;
    return separation;
}

glm::vec3 scene::PaperPlane::alignment(renderer::Models_t &models, size_t &size) {
    auto findAligment = [this](glm::vec3 velocity, std::unique_ptr<renderer::Model> &model) {
        if (this->getId() == model->getId() || model->getModelType() != renderer::PAPER_PLANE)
            return velocity;
        else if (this->getId() == 0 && model->getId() == 1)
            return model->getVelocity();
        return velocity + model->getVelocity();
    };
    glm::vec3 alignment = std::accumulate(std::next(models.begin()), models.end(),
                                          models.front()->getVelocity(), findAligment);
    alignment /= size;
    alignment *= 3;
    return alignment;
}

glm::vec3 scene::PaperPlane::collision(renderer::Models_t &models) {
    glm::vec3 collision = glm::vec3(0.0f, 0.0f, 0.0f);
    for (auto &model : models)
        if (model->getModelType() != renderer::PAPER_PLANE)
            if (willCollide(model->getPosition()))
                collision -= model->getPosition() - _position;
    return collision;
}

glm::vec3 scene::PaperPlane::boundaries() {
    glm::vec3 bound = glm::vec3(0.0f, 0.0f, 0.0f);
    if (_position.x < -10)
        bound.x = 1;
    else if (_position.x > 10)
        bound.x = -1;
    if (_position.y < -10)
        bound.y = 1;
    else if (_position.y > 10)
        bound.y = -1;
    if (_position.z < 0)
        bound.z = 1;
    else if (_position.z > 30)
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

bool scene::PaperPlane::willCollide(glm::vec3 position) {
    return (glm::length(position - _position) < _separationMin);
}

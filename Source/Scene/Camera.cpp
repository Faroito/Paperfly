//
// Created by Timothée Couble on 10/12/2019.
//

#include "Camera.hpp"

scene::Camera::Camera() {
    updateCamerasVectors();
}

void scene::Camera::updateCamerasVectors() {
    _cameraRight = glm::normalize(glm::cross(_cameraFront, _worldUp));
    _cameraUp = glm::normalize(glm::cross(_cameraRight, _cameraFront));
}


const glm::vec3 &scene::Camera::getCameraPosition() const {
    return _cameraPos;
}

glm::mat4 scene::Camera::getViewMatrix() const {
    return glm::lookAt(_cameraPos, _cameraPos + _cameraFront, _cameraUp);
}

glm::mat4 scene::Camera::getProjectionMatrix(float ratio) const {
    auto projection = glm::perspective(glm::radians(_fov), ratio, 0.1f, 50.0f);
    projection[1][1] *= -1;
    return projection;
}

void scene::Camera::zoom(double yOffset) {
    _fov -= (float) yOffset / 2;
    if (_fov <= 1.0f)
        _fov = 1.0f;
    else if(_fov >= 45.0f)
        _fov = 45.0f;
}

void scene::Camera::moveForward() {
    _cameraPos += _cameraFront * _cameraSpeed;
}

void scene::Camera::moveBackward() {
    _cameraPos -= _cameraFront * _cameraSpeed;
}

void scene::Camera::moveUp() {
    _cameraPos += _cameraUp * _cameraSpeed;
}

void scene::Camera::moveDown() {
    _cameraPos -= _cameraUp * _cameraSpeed;
}

void scene::Camera::moveLeft() {
    _cameraPos -= _cameraRight * _cameraSpeed;
}

void scene::Camera::moveRight() {
    _cameraPos += _cameraRight * _cameraSpeed;
}

void scene::Camera::rotateLeft() {
    _cameraFront = glm::angleAxis(_cameraSpeed / 6, _cameraUp) * _cameraFront;
    updateCamerasVectors();
}

void scene::Camera::rotateRight() {
    _cameraFront = glm::angleAxis(-_cameraSpeed / 6, _cameraUp) * _cameraFront;
    updateCamerasVectors();
}
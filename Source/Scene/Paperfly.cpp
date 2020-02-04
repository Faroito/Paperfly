//
// Created by Timothée Couble on 10/12/2019.
//

#include "Paperfly.hpp"

scene::Paperfly::Paperfly() : renderer::Application("Paperfly") {
    initModels();
}

void scene::Paperfly::initModels() {
    _models.push_back(renderer::Model(renderer::PAPER_PLANE, renderer::BLUE));
    _models.push_back(renderer::Model(renderer::PAPER_PLANE, renderer::ORANGE));
    _models.push_back(renderer::Model(renderer::PAPER_PLANE, renderer::BLUE));
    _models.push_back(renderer::Model(renderer::PAPER_PLANE, renderer::ORANGE));
    _models.push_back(renderer::Model(renderer::PAPER_PLANE, renderer::BLUE));
    _models[0].setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    _models[1].setPosition(glm::vec3(1.0f, 0.0f, 0.0f));
    _models[2].setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
    _models[3].setPosition(glm::vec3(1.0f, 1.0f, 0.0f));
    _models[4].setPosition(glm::vec3(0.0f, 2.0f, 0.0f));

    Application::initModels();
}

void scene::Paperfly::onDraw() {
    checkKey();

    for (auto &model : _models)
        model.setCamera(_camera, _swapChain.ratio());
    Application::onDraw();
}

void scene::Paperfly::checkKey() {
    for (const auto &it : _keyMap) {
        if (_keyCode[it.first] && _pressed)
            (_camera.get()->*it.second)();
    }
    if (_keyCode[GLFW_KEY_ESCAPE])
        _window.close();
}

void scene::Paperfly::onMouseMove(double x, double y) {
    // std::cout << x << " " << y << std::endl;
}

void scene::Paperfly::onMouseDown(int button, int action) {
    if (action == GLFW_PRESS)
        std::cout << "Clicked on: " << button << std::endl;
}

void scene::Paperfly::onMouseScroll(double x, double y) {
    _camera->zoom(y);
}

void scene::Paperfly::onKeyDown(int key, int action) {
    if (action == GLFW_PRESS) {
        _pressed = true;
        _keyCode[key] = true;
    } if (action == GLFW_RELEASE) {
        _pressed = false;
        _keyCode[key] = false;
    }
}
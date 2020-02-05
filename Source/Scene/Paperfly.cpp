//
// Created by Timothée Couble on 10/12/2019.
//

#include "Paperfly.hpp"

scene::Paperfly::Paperfly() : renderer::Application("Paperfly") {
    initModels();
    std::srand(std::time(nullptr));
}

void scene::Paperfly::initModels() {

    const size_t planeNb = 30;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

    auto findBlock = [&position](std::unique_ptr<renderer::Model> &obj) { return obj->getPosition() == position; };
    for (size_t i = 0; i < planeNb; i++) {
        do {
            position = glm::vec3(rand() % 7 - 3, (rand() % 13 - 6) * 0.5, (rand() % 5) * 1.5);
        } while (std::find_if(std::begin(_models), std::end(_models), findBlock) != std::end(_models));
        _models.push_back(std::make_unique<PaperPlane>(PaperPlane(renderer::COLORS_AVAILABLE[i % 7])));
        _models.back()->setPosition(position);
    }

    Application::initModels();
}

void scene::Paperfly::onDraw() {
    checkKey();

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    for (auto &model : _models) {
        if (time > 0.02) {
            startTime = std::chrono::high_resolution_clock::now();
            model->update(_models);
        }
        model->setCamera(_camera, _swapChain.ratio());
    }
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
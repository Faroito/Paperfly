//
// Created by Timothée Couble on 10/12/2019.
//

#include "Paperfly.hpp"

scene::Paperfly::Paperfly() : renderer::Application("Paperfly") {}

void scene::Paperfly::onDraw() {
    checkKey();

    _model.setCamera(_camera, _swapChain.ratio());
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
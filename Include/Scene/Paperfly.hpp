//
// Created by Timothée Couble on 10/12/2019.
//

#ifndef PAPERFLY_HPP
# define PAPERFLY_HPP

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <algorithm>
#include <unordered_map>

#include "Application.hpp"
#include "Camera.hpp"
#include "Cylinder.hpp"
#include "PaperPlane.hpp"

namespace scene {
    class Paperfly : public renderer::Application {
    public:
        Paperfly();

    private:
        void initModels() override;

        void onDraw() override;

        void checkKey();

        void onMouseMove(double x, double y) final;
        void onMouseDown(int button, int action) final;
        void onMouseScroll(double x, double y) final;
        void onKeyDown(int key, int action) final;

    private:
        bool _pressed = false;
        bool _keyCode[512] = { false };
        Camera_ptr_t _camera = std::make_unique<Camera>(Camera());
        const std::unordered_map<int, change_camera_t> _keyMap = {
                {GLFW_KEY_W, &scene::Camera::moveForward},
                {GLFW_KEY_S, &scene::Camera::moveBackward},
                {GLFW_KEY_R, &scene::Camera::moveUp},
                {GLFW_KEY_F, &scene::Camera::moveDown},
                {GLFW_KEY_A, &scene::Camera::moveLeft},
                {GLFW_KEY_D, &scene::Camera::moveRight},
                {GLFW_KEY_Q, &scene::Camera::rotateLeft},
                {GLFW_KEY_E, &scene::Camera::rotateRight}
        };

    };
}

#endif /* !PAPERFLY_HPP */

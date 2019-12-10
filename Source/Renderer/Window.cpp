//
// Created by Timothée Couble on 10/12/2019.
//

#include "Window.hpp"

void renderer::Window::setUp(const std::string &appName) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    _window = glfwCreateWindow(WIDTH, HEIGHT, appName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);

}

void renderer::Window::cleanUp() {
    glfwDestroyWindow(_window);
    glfwTerminate();
}

GLFWwindow *renderer::Window::get() {
    return _window;
}

bool renderer::Window::shouldClose() {
    return glfwWindowShouldClose(_window);
}

void renderer::Window::resize() {
    int width = 0, height = 0;
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(_window, &width, &height);
        if (width == 0 || height == 0)
            glfwWaitEvents();
    }
}

void renderer::Window::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    app->resized = true;
}


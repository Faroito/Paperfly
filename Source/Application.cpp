//
// Created by Timothée Couble on 08/12/2019.
//

#include "Application.hpp"

void renderer::Application::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void renderer::Application::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    _window = glfwCreateWindow(WIDTH, HEIGHT, _appName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
}

void renderer::Application::initVulkan() {
    _instance.setUp(_appName);
    _debugMessenger.setUp(_instance.get());
    _surface.setUp(_instance.get(), _window);
    _devices.setUp(_instance.get(), _surface);
    _swapChain.setUp(_window, _surface, _devices);
    _pipeline.setUp(_devices, _swapChain);
    _commandPool.setUp(_devices.get(), _surface.findQueueFamilies(_devices.getPhysical()));
    _depthImage.setUp(_devices, _swapChain.getExtent());
    _framebuffers.setUp(_devices.get(), _swapChain, _pipeline.getRenderPass(), _depthImage.get());
    _model.setUp(_devices, _commandPool.get());
    _model.setUpSwapChain(_devices, _swapChain, _pipeline, _framebuffers, _commandPool.get());
    _syncObjects.setUp(_devices.get(), _swapChain.size());
}

void renderer::Application::mainLoop() {
    while (!glfwWindowShouldClose(_window)) {
        glfwPollEvents();
        if (_syncObjects.drawFrame(_devices, _swapChain, _model, _framebufferResized)) {
            _framebufferResized = false;
            recreateSwapChain();
        }
    }

    vkDeviceWaitIdle(_devices.get());
}

void renderer::Application::cleanup() {
    cleanupSwapChain();

    _model.cleanUp(_devices.get());
    _syncObjects.cleanUp(_devices.get());
    _commandPool.cleanUp(_devices.get());
    _devices.cleanUp();
    _debugMessenger.cleanUp(_instance.get());
    _surface.cleanUp(_instance.get());
    _instance.cleanUp();

    glfwDestroyWindow(_window);
    glfwTerminate();
}

void renderer::Application::recreateSwapChain() {
    int width = 0, height = 0;
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(_window, &width, &height);
        if (width == 0 || height == 0)
            glfwWaitEvents();
    }
    vkDeviceWaitIdle(_devices.get());

    cleanupSwapChain();

    _swapChain.setUp(_window, _surface, _devices);
    _pipeline.setUp(_devices, _swapChain);
    _depthImage.setUp(_devices, _swapChain.getExtent());
    _framebuffers.setUp(_devices.get(), _swapChain, _pipeline.getRenderPass(), _depthImage.get());
    _model.setUpSwapChain(_devices, _swapChain, _pipeline, _framebuffers, _commandPool.get());
}

void renderer::Application::cleanupSwapChain() {
    _depthImage.cleanUp(_devices.get());
    _framebuffers.cleanUp(_devices.get());
    _model.cleanUpSwapChain(_devices.get(), _commandPool.get());
    _pipeline.cleanUp(_devices.get());
    _swapChain.cleanUp(_devices.get());
}

void renderer::Application::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->_framebufferResized = true;
}

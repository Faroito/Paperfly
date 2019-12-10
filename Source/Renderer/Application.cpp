//
// Created by Timothée Couble on 08/12/2019.
//

#include "Application.hpp"

renderer::Application::Application(const std::string &appName) : _appName(appName) {
    _window.setUp(_appName);
    initVulkan();
}

renderer::Application::~Application() {
    cleanup();
}

void renderer::Application::initVulkan() {
    _instance.setUp(_appName);
    _debugMessenger.setUp(_instance.get());
    _surface.setUp(_instance.get(), _window.get());
    _devices.setUp(_instance.get(), _surface);
    _swapChain.setUp(_window.get(), _surface, _devices);
    _pipeline.setUp(_devices, _swapChain);
    _commandPool.setUp(_devices.get(), _surface.findQueueFamilies(_devices.getPhysical()));
    _depthImage.setUp(_devices, _swapChain.getExtent());
    _framebuffers.setUp(_devices.get(), _swapChain, _pipeline.getRenderPass(), _depthImage.get());
    _model.setUp(_devices, _commandPool.get());
    _model.setUpSwapChain(_devices, _swapChain, _pipeline, _framebuffers, _commandPool.get());
    _syncObjects.setUp(_devices.get(), _swapChain.size());
}

void renderer::Application::run() {
    while (!_window.shouldClose()) {
        glfwPollEvents();
        this->onDraw();
    }

    vkDeviceWaitIdle(_devices.get());
}

void renderer::Application::onDraw() {
    if (_syncObjects.drawFrame(_devices, _swapChain, _model, _window.resized)) {
        _window.resized = false;
        recreateSwapChain();
    }
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
    _window.cleanUp();
}

void renderer::Application::cleanupSwapChain() {
    _depthImage.cleanUp(_devices.get());
    _framebuffers.cleanUp(_devices.get());
    _model.cleanUpSwapChain(_devices.get(), _commandPool.get());
    _pipeline.cleanUp(_devices.get());
    _swapChain.cleanUp(_devices.get());
}

void renderer::Application::recreateSwapChain() {
    _window.resize();
    vkDeviceWaitIdle(_devices.get());

    cleanupSwapChain();

    _swapChain.setUp(_window.get(), _surface, _devices);
    _pipeline.setUp(_devices, _swapChain);
    _depthImage.setUp(_devices, _swapChain.getExtent());
    _framebuffers.setUp(_devices.get(), _swapChain, _pipeline.getRenderPass(), _depthImage.get());
    _model.setUpSwapChain(_devices, _swapChain, _pipeline, _framebuffers, _commandPool.get());
}

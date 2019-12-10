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
    _commands.setUpPool(_devices.get(), _surface.findQueueFamilies(_devices.getPhysical()));
    createDepthResources();
    _framebuffers.setUp(_devices.get(), _swapChain, _pipeline.getRenderPass(), _depthImageView);
    _model.setUp(_devices, _commands.getPool());
    _uniforms.setUp(_devices, _swapChain.size());
    _commands.setUpBuffers(_devices.get(), _swapChain, _pipeline, _framebuffers, _model, _uniforms);
    createSyncObjects();
}

void renderer::Application::mainLoop() {
    while (!glfwWindowShouldClose(_window)) {
        glfwPollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(_devices.get());
}

void renderer::Application::drawFrame() {
    vkWaitForFences(_devices.get(), 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(_devices.get(), _swapChain.get(), UINT64_MAX,
                                            _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("failed to acquire swap chain image!");

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (_imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(_devices.get(), 1, &_imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    // Mark the image as now being in use by this frame
    _imagesInFlight[imageIndex] = _inFlightFences[_currentFrame];

    updateUniformBuffer(imageIndex);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {_imageAvailableSemaphores[_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commands[imageIndex];

    VkSemaphore signalSemaphores[] = {_renderFinishedSemaphores[_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(_devices.get(), 1, &_inFlightFences[_currentFrame]);
    if (vkQueueSubmit(_devices.getGraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]) != VK_SUCCESS)
        throw std::runtime_error("failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {_swapChain.get()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(_devices.getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebufferResized) {
        _framebufferResized = false;
        recreateSwapChain();
    } else if (result != VK_SUCCESS)
        throw std::runtime_error("failed to present swap chain image!");

    vkQueueWaitIdle(_devices.getPresentQueue());

    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void renderer::Application::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo = {};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), _swapChain.ratio(), 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    void* data;
    vkMapMemory(_devices.get(), _uniforms.getMemory(currentImage), 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(_devices.get(), _uniforms.getMemory(currentImage));
}

void renderer::Application::cleanup() {
    cleanupSwapChain();

    _model.cleanUp(_devices.get());

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(_devices.get(), _renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(_devices.get(), _imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(_devices.get(), _inFlightFences[i], nullptr);
    }

    _commands.cleanUpPool(_devices.get());
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
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(_devices.get());

    cleanupSwapChain();

    _swapChain.setUp(_window, _surface, _devices);
    _pipeline.setUp(_devices, _swapChain);
    createDepthResources();
    _framebuffers.setUp(_devices.get(), _swapChain, _pipeline.getRenderPass(), _depthImageView);
    _uniforms.setUp(_devices, _swapChain.size());
    _commands.setUpBuffers(_devices.get(), _swapChain, _pipeline, _framebuffers, _model, _uniforms);

}

void renderer::Application::cleanupSwapChain() {
    vkDestroyImageView(_devices.get(), _depthImageView, nullptr);
    vkDestroyImage(_devices.get(), _depthImage, nullptr);
    vkFreeMemory(_devices.get(), _depthImageMemory, nullptr);

    _framebuffers.cleanUp(_devices.get());
    _commands.cleanUpBuffers(_devices.get());
    _pipeline.cleanUp(_devices.get());
    _swapChain.cleanUp(_devices.get());
    _uniforms.cleanUp(_devices.get());
}

void renderer::Application::createDepthResources() {
    VkFormat depthFormat = findDepthFormat(_devices.getPhysical());

    BufferManip::createImage(_devices, _swapChain.getExtent().width, _swapChain.getExtent().height, depthFormat,
            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _depthImage, _depthImageMemory);
    _depthImageView = BufferManip::createImageView(_devices.get(), _depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void renderer::Application::createSyncObjects() {
    _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    _imagesInFlight.resize(_swapChain.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(_devices.get(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(_devices.get(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(_devices.get(), &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
}

void renderer::Application::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->_framebufferResized = true;
}

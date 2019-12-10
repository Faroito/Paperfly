//
// Created by Timothée Couble on 08/12/2019.
//

#ifndef APPLICATION_HPP
# define APPLICATION_HPP

#include <iostream>
#include <functional>
#include <vector>
#include <chrono>

#include "BufferManip.hpp"
#include "CommandBuffers.hpp"
#include "Debug.hpp"
#include "Devices.hpp"
#include "Framebuffers.hpp"
#include "GraphicsPipeline.hpp"
#include "Instance.hpp"
#include "Model.hpp"
#include "Libraries.hpp"
#include "Surface.hpp"
#include "SwapChain.hpp"
#include "UniformBuffers.hpp"
#include "Utils.hpp"
#include "Vertex.hpp"

namespace renderer {
    class Application {
    public:
        Application() = default;
        void run();

    private:
        void initWindow();
        void initVulkan();
        void mainLoop();
        void cleanup();

        void drawFrame();
        void updateUniformBuffer(uint32_t currentImage);

        void recreateSwapChain();
        void cleanupSwapChain();

        void createDepthResources();
        void createSyncObjects();

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    private:
        const std::string _appName = "Paperfly";
        GLFWwindow* _window = nullptr;

        Instance _instance;
        Debug _debugMessenger;
        Surface _surface;
        Devices _devices;
        SwapChain _swapChain;
        GraphicsPipeline _pipeline;
        Framebuffers _framebuffers;
        CommandBuffers _commands;
        UniformBuffers _uniforms;
        Model _model;

        VkImage _depthImage;
        VkDeviceMemory _depthImageMemory;
        VkImageView _depthImageView;

        size_t _currentFrame = 0;
        std::vector<VkSemaphore> _imageAvailableSemaphores;
        std::vector<VkSemaphore> _renderFinishedSemaphores;
        std::vector<VkFence> _inFlightFences;
        std::vector<VkFence> _imagesInFlight;

        bool _framebufferResized = false;
    };
};

#endif /* !APPLICATION_HPP */
//
// Created by Timothée Couble on 08/12/2019.
//

#ifndef APPLICATION_HPP
# define APPLICATION_HPP

#include <iostream>
#include <functional>
#include <vector>

#include "BufferManip.hpp"
#include "CommandPool.hpp"
#include "Debug.hpp"
#include "DepthImage.hpp"
#include "Devices.hpp"
#include "Framebuffers.hpp"
#include "GraphicsPipeline.hpp"
#include "Instance.hpp"
#include "Libraries.hpp"
#include "Model.hpp"
#include "Surface.hpp"
#include "SwapChain.hpp"
#include "SyncObjects.hpp"
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

        void recreateSwapChain();
        void cleanupSwapChain();

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
        CommandPool _commandPool;
        DepthImage _depthImage;
        Framebuffers _framebuffers;
        Model _model;
        SyncObjects _syncObjects;

        bool _framebufferResized = false;
    };
};

#endif /* !APPLICATION_HPP */
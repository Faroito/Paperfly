//
// Created by Timothée Couble on 08/12/2019.
//

#ifndef APPLICATION_HPP
# define APPLICATION_HPP

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
#include "Window.hpp"

namespace renderer {
    class Application {
    public:
        explicit Application(const std::string &appName);
        ~Application();

        void run();

    protected:
        virtual void onDraw();

    private:
        void initVulkan();

        void cleanup();
        void cleanupSwapChain();
        void recreateSwapChain();

    protected:
        Window _window;
        Model _model;

    private:
        const std::string _appName;

        Instance _instance;
        Debug _debugMessenger;
        Surface _surface;
        Devices _devices;
        SwapChain _swapChain;
        GraphicsPipeline _pipeline;
        CommandPool _commandPool;
        DepthImage _depthImage;
        Framebuffers _framebuffers;
        SyncObjects _syncObjects;
    };
};

#endif /* !APPLICATION_HPP */
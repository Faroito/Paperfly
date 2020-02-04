//
// Created by Timothée Couble on 08/12/2019.
//

#ifndef APPLICATION_HPP
# define APPLICATION_HPP

#include "IApplication.hpp"
#include "BufferManip.hpp"
#include "CommandPool.hpp"
#include "CommandBuffers.hpp"
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
    class Application : public IApplication {
    public:
        explicit Application(const std::string &appName);
        ~Application();

        void run();

    protected:
        virtual void onDraw();

        void onMouseMove(double x, double y) override;
        void onMouseDown(int button, int action) override;
        void onMouseScroll(double x, double y) override;
        void onKeyDown(int key, int action) override;

    private:
        void initVulkan();

        void cleanup();
        void cleanupSwapChain();
        void recreateSwapChain();

    protected:
        Window _window;
        SwapChain _swapChain;
        Model _model;
        Model _model2 = Model("pp_cyan_texture.png");

    private:
        const std::string _appName;

        Instance _instance;
        Debug _debugMessenger;
        Surface _surface;
        Devices _devices;
        GraphicsPipeline _pipeline;
        CommandPool _commandPool;
        CommandBuffers _commandBuffers;
        DepthImage _depthImage;
        Framebuffers _framebuffers;
        SyncObjects _syncObjects;
    };
};

#endif /* !APPLICATION_HPP */
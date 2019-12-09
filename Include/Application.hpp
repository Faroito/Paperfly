//
// Created by Timothée Couble on 08/12/2019.
//

#ifndef APPLICATION_HPP
# define APPLICATION_HPP

#include <iostream>
#include <stdexcept>
#include <functional>
#include <optional>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <unordered_map>

#include "Debug.hpp"
#include "Devices.hpp"
#include "Instance.hpp"
#include "Libraries.hpp"
#include "Surface.hpp"
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

        void createSwapChain();
        void recreateSwapChain();
        void cleanupSwapChain();
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        void createImageViews();

        void createRenderPass();
        void createDescriptorSetLayout();
        void createGraphicsPipeline();
        VkShaderModule createShaderModule(const std::vector<char>& code);

        void createFramebuffers();
        void createCommandPool();

        void createDepthResources();
        VkFormat findDepthFormat();
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
                VkImageTiling tiling, VkFormatFeatureFlags features);

        void createTextureImage();
        void createTextureImageView();
        void createTextureSampler();
        void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

        void loadModel();
        void createVertexBuffer();
        void createIndexBuffer();
        void createUniformBuffers();
        void createDescriptorPool();
        void createDescriptorSets();
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void createCommandBuffers();

        void createSyncObjects();

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    private:
        const std::string _appName = "Paperfly";
        GLFWwindow* _window;

        Instance _instance;
        Debug _debugMessenger;
        Surface _surface;
        Devices _devices;

        VkSwapchainKHR _swapChain;
        std::vector<VkImage> _swapChainImages;
        VkFormat _swapChainImageFormat;
        VkExtent2D _swapChainExtent;
        std::vector<VkImageView> _swapChainImageViews;

        VkRenderPass _renderPass;
        VkDescriptorSetLayout _descriptorSetLayout;
        VkPipelineLayout _pipelineLayout;
        VkPipeline _graphicsPipeline;

        std::vector<VkFramebuffer> _swapChainFramebuffers;
        VkCommandPool _commandPool;
        std::vector<VkCommandBuffer> _commandBuffers;

        VkImage _depthImage;
        VkDeviceMemory _depthImageMemory;
        VkImageView _depthImageView;

        VkImage _textureImage;
        VkDeviceMemory _textureImageMemory;
        VkImageView _textureImageView;
        VkSampler _textureSampler;

        std::vector<Vertex> _vertices;
        std::vector<uint32_t> _indices;

        VkBuffer _vertexBuffer;
        VkDeviceMemory _vertexBufferMemory;
        VkBuffer _indexBuffer;
        VkDeviceMemory _indexBufferMemory;

        std::vector<VkBuffer> _uniformBuffers;
        std::vector<VkDeviceMemory> _uniformBuffersMemory;

        VkDescriptorPool _descriptorPool;
        std::vector<VkDescriptorSet> _descriptorSets;

        size_t _currentFrame = 0;
        std::vector<VkSemaphore> _imageAvailableSemaphores;
        std::vector<VkSemaphore> _renderFinishedSemaphores;
        std::vector<VkFence> _inFlightFences;
        std::vector<VkFence> _imagesInFlight;

        bool _framebufferResized = false;
    };
};

#endif /* !APPLICATION_HPP */
//
// Created by Timothée Couble on 08/12/2019.
//

#include <stb_image.h>
#include <tiny_obj_loader.h>

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
    createCommandPool();
    createDepthResources();
    _framebuffers.setUp(_devices.get(), _swapChain, _pipeline.getRenderPass(), _depthImageView);
    createTextureImage();
    createTextureImageView();
    createTextureSampler();
    loadModel();
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
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
    submitInfo.pCommandBuffers = &_commandBuffers[imageIndex];

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
    vkMapMemory(_devices.get(), _uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(_devices.get(), _uniformBuffersMemory[currentImage]);
}

void renderer::Application::cleanup() {
    cleanupSwapChain();

    vkDestroySampler(_devices.get(), _textureSampler, nullptr);
    vkDestroyImageView(_devices.get(), _textureImageView, nullptr);

    vkDestroyImage(_devices.get(), _textureImage, nullptr);
    vkFreeMemory(_devices.get(), _textureImageMemory, nullptr);

    vkDestroyBuffer(_devices.get(), _vertexBuffer, nullptr);
    vkFreeMemory(_devices.get(), _vertexBufferMemory, nullptr);
    vkDestroyBuffer(_devices.get(), _indexBuffer, nullptr);
    vkFreeMemory(_devices.get(), _indexBufferMemory, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(_devices.get(), _renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(_devices.get(), _imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(_devices.get(), _inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(_devices.get(), _commandPool, nullptr);
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
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();

}

void renderer::Application::cleanupSwapChain() {
    vkDestroyImageView(_devices.get(), _depthImageView, nullptr);
    vkDestroyImage(_devices.get(), _depthImage, nullptr);
    vkFreeMemory(_devices.get(), _depthImageMemory, nullptr);

    _framebuffers.cleanUp(_devices.get());

    vkFreeCommandBuffers(_devices.get(), _commandPool,
            static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());

    _pipeline.cleanUp(_devices.get());
    _swapChain.cleanUp(_devices.get());

    for (size_t i = 0; i < _swapChain.size(); i++) {
        vkDestroyBuffer(_devices.get(), _uniformBuffers[i], nullptr);
        vkFreeMemory(_devices.get(), _uniformBuffersMemory[i], nullptr);
    }

    vkDestroyDescriptorPool(_devices.get(), _descriptorPool, nullptr);
}

void renderer::Application::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = _surface.findQueueFamilies(_devices.getPhysical());

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = 0;

    if (vkCreateCommandPool(_devices.get(), &poolInfo, nullptr, &_commandPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create command pool!");
}

void renderer::Application::createDepthResources() {
    VkFormat depthFormat = findDepthFormat(_devices.getPhysical());

    createImage(_swapChain.getExtent().width, _swapChain.getExtent().height, depthFormat,
            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _depthImage, _depthImageMemory);
    _depthImageView = createImageView(_devices.get(), _depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void renderer::Application::createTextureImage() {
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;
    if (!pixels)
        throw std::runtime_error("failed to load texture image!");

    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(_devices.get(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(_devices.get(), stagingBufferMemory);

    stbi_image_free(pixels);

    createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _textureImage, _textureImageMemory);

    transitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffer, _textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

    transitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(_devices.get(), stagingBuffer, nullptr);
    vkFreeMemory(_devices.get(), stagingBufferMemory, nullptr);
}

void renderer::Application::createTextureImageView() {
    _textureImageView = createImageView(_devices.get(), _textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
}

void renderer::Application::createTextureSampler() {
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(_devices.get(), &samplerInfo, nullptr, &_textureSampler) != VK_SUCCESS)
        throw std::runtime_error("failed to create texture sampler!");
}

void renderer::Application::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                              VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image,
                              VkDeviceMemory &imageMemory) {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(_devices.get(), &imageInfo, nullptr, &image) != VK_SUCCESS)
        throw std::runtime_error("failed to create image!");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(_devices.get(), image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(_devices.get(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate image memory!");

    vkBindImageMemory(_devices.get(), image, imageMemory, 0);
}

void renderer::Application::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(commandBuffer);
}

void renderer::Application::transitionImageLayout(VkImage image, VkFormat format,
        VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
            newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else
        throw std::invalid_argument("unsupported layout transition!");

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    endSingleTimeCommands(commandBuffer);
}

void renderer::Application::loadModel() {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str()))
        throw std::runtime_error(warn + err);

    std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            Vertex vertex = {};

            vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = {1.0f, 1.0f, 1.0f};

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(_vertices.size());
                _vertices.push_back(vertex);
            }

            _indices.push_back(uniqueVertices[vertex]);
        }
    }
}

void renderer::Application::createVertexBuffer() {
    VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(_devices.get(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, _vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(_devices.get(), stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer, _vertexBufferMemory);

    copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

    vkDestroyBuffer(_devices.get(), stagingBuffer, nullptr);
    vkFreeMemory(_devices.get(), stagingBufferMemory, nullptr);
}

void renderer::Application::createIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(_indices[0]) * _indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(_devices.get(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, _indices.data(), (size_t) bufferSize);
    vkUnmapMemory(_devices.get(), stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);

    copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

    vkDestroyBuffer(_devices.get(), stagingBuffer, nullptr);
    vkFreeMemory(_devices.get(), stagingBufferMemory, nullptr);
}

void renderer::Application::createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    _uniformBuffers.resize(_swapChain.size());
    _uniformBuffersMemory.resize(_swapChain.size());

    for (size_t i = 0; i < _swapChain.size(); i++)
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                _uniformBuffers[i], _uniformBuffersMemory[i]);
}

void renderer::Application::createDescriptorPool() {
    std::array<VkDescriptorPoolSize, 2> poolSizes = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(_swapChain.size());
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(_swapChain.size());

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(_swapChain.size());

    if (vkCreateDescriptorPool(_devices.get(), &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create descriptor pool!");
}

void renderer::Application::createDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(_swapChain.size(), _pipeline.getDescriptorSetLayout());
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = _descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(_swapChain.size());
    allocInfo.pSetLayouts = layouts.data();

    _descriptorSets.resize(_swapChain.size());
    if (vkAllocateDescriptorSets(_devices.get(), &allocInfo, _descriptorSets.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate descriptor sets!");

    for (size_t i = 0; i < _swapChain.size(); i++) {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = _uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = _textureImageView;
        imageInfo.sampler = _textureSampler;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = _descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = _descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(_devices.get(),
                               static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void renderer::Application::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(_devices.get(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(_devices.get(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(_devices.get(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate buffer memory!");

    vkBindBufferMemory(_devices.get(), buffer, bufferMemory, 0);
}

void renderer::Application::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion = {};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

VkCommandBuffer renderer::Application::beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = _commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(_devices.get(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void renderer::Application::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(_devices.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_devices.getGraphicsQueue());

    vkFreeCommandBuffers(_devices.get(), _commandPool, 1, &commandBuffer);
}

uint32_t renderer::Application::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(_devices.getPhysical(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void renderer::Application::createCommandBuffers() {
    _commandBuffers.resize(_framebuffers.size());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = _commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) _commandBuffers.size();

    if (vkAllocateCommandBuffers(_devices.get(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for (size_t i = 0; i < _commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(_commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = _pipeline.getRenderPass();
        renderPassInfo.framebuffer = _framebuffers[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = _swapChain.getExtent();

        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = {0.07f, 0.17f, 0.24f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.get());

        VkBuffer vertexBuffers[] = {_vertexBuffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(_commandBuffers[i], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(_commandBuffers[i], _indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                _pipeline.getLayout(), 0, 1, &_descriptorSets[i], 0, nullptr);
        vkCmdDrawIndexed(_commandBuffers[i], static_cast<uint32_t>(_indices.size()), 1, 0, 0, 0);

        vkCmdEndRenderPass(_commandBuffers[i]);

        if (vkEndCommandBuffer(_commandBuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to record command buffer!");
    }
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

//
// Created by Timothée Couble on 09/12/2019.
//

#include <stb_image.h>

#include "Texture.hpp"

renderer::Texture::Texture(std::string path, ModelColor color) : _color(color), _path(path) {}

void renderer::Texture::setUp(renderer::Devices &devices, VkCommandPool &pool) {
    createTextureImage(devices, pool, _path);
    createTextureImageView(devices.get());
    createTextureSampler(devices.get());
}

void renderer::Texture::createTextureImage(Devices &devices, VkCommandPool &pool, const std::string &textureName) {
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(textureName.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;
    if (!pixels)
        throw std::runtime_error("failed to load texture image!");

    BufferManip::createBuffer(devices, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(devices.get(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(devices.get(), stagingBufferMemory);

    stbi_image_free(pixels);

    BufferManip::createImage(devices, texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _textureImage, _textureImageMemory);

    BufferManip::transitionImageLayout(devices, pool, _textureImage, VK_FORMAT_R8G8B8A8_UNORM,
                          VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    BufferManip::copyBufferToImage(devices, pool, stagingBuffer, _textureImage,
            static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

    BufferManip::transitionImageLayout(devices, pool, _textureImage, VK_FORMAT_R8G8B8A8_UNORM,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(devices.get(), stagingBuffer, nullptr);
    vkFreeMemory(devices.get(), stagingBufferMemory, nullptr);

}

void renderer::Texture::createTextureImageView(VkDevice &device) {
    _textureImageView = BufferManip::createImageView(device, _textureImage, VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_ASPECT_COLOR_BIT);
}

void renderer::Texture::createTextureSampler(VkDevice &device) {
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

    if (vkCreateSampler(device, &samplerInfo, nullptr, &_textureSampler) != VK_SUCCESS)
        throw std::runtime_error("failed to create texture sampler!");
}


void renderer::Texture::cleanUp(VkDevice &device) {
    vkDestroySampler(device, _textureSampler, nullptr);
    vkDestroyImageView(device, _textureImageView, nullptr);
    vkDestroyImage(device, _textureImage, nullptr);
    vkFreeMemory(device, _textureImageMemory, nullptr);
}

VkImageView &renderer::Texture::getImageView() {
    return _textureImageView;
}

VkSampler &renderer::Texture::getSampler() {
    return _textureSampler;
}

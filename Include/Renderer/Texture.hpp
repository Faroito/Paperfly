//
// Created by Timothée Couble on 09/12/2019.
//

#ifndef TEXTURE_HPP
# define TEXTURE_HPP

#include "BufferManip.hpp"
#include "Devices.hpp"
#include "Libraries.hpp"
#include "Utils.hpp"

namespace renderer {

    class Texture {
    public:
        Texture() = default;

        void setUp(Devices &devices, VkCommandPool &pool);
        void cleanUp(VkDevice &device);

        VkImageView &getImageView();
        VkSampler &getSampler();

    private:
        void createTextureImage(Devices &devices, VkCommandPool &pool);
        void createTextureImageView(VkDevice &device);
        void createTextureSampler(VkDevice &device);

    private:
        VkImage _textureImage;
        VkDeviceMemory _textureImageMemory;
        VkImageView _textureImageView;
        VkSampler _textureSampler;

    };
}

#endif /* !TEXTURE_HPP */

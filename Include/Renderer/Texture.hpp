//
// Created by Timothée Couble on 09/12/2019.
//

#ifndef TEXTURE_HPP
# define TEXTURE_HPP

#include <unordered_map>
#include <string>

#include "BufferManip.hpp"
#include "Devices.hpp"
#include "Libraries.hpp"
#include "Utils.hpp"

namespace renderer {

    class Texture {
    public:
        explicit Texture(ModelColor color);

        void setUp(Devices &devices, VkCommandPool &pool);
        void cleanUp(VkDevice &device);

        VkImageView &getImageView();
        VkSampler &getSampler();

    private:
        void createTextureImage(Devices &devices, VkCommandPool &pool, const std::string &textureName);
        void createTextureImageView(VkDevice &device);
        void createTextureSampler(VkDevice &device);

    private:
        VkImage _textureImage;
        VkDeviceMemory _textureImageMemory;
        VkImageView _textureImageView;
        VkSampler _textureSampler;

        const ModelColor _color;
        const std::unordered_map<ModelColor, std::string> _textureFile = {
                {ModelColor::BLUE, "pp_blue_texture.png"},
                {ModelColor::ORANGE, "pp_orange_texture.png"},
                {ModelColor::PURPLE, "pp_purple_texture.png"},
                {ModelColor::CYAN, "pp_cyan_texture.png"},
                {ModelColor::RED, "pp_red_texture.png"},
                {ModelColor::GREEN, "pp_green_texture.png"},
                {ModelColor::YELLOW, "pp_yellow_texture.png"}
        };
    };

    typedef std::unordered_map<ModelColor, Texture> TextureMap_t;
}

#endif /* !TEXTURE_HPP */

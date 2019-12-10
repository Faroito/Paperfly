//
// Created by Timothée Couble on 10/12/2019.
//

#ifndef DEPTH_IMAGE_HPP
# define DEPTH_IMAGE_HPP

#include "BufferManip.hpp"
#include "Devices.hpp"
#include "Libraries.hpp"
#include "Utils.hpp"

namespace renderer {

    /**
     * A depth attachment is based on an image.
     */
    class DepthImage {
    public:
        DepthImage() = default;

        void setUp(Devices &devices, VkExtent2D &extent);
        void cleanUp(VkDevice &device);

        VkImageView &get();

    private:
        VkImage _depthImage;
        VkDeviceMemory _depthImageMemory;
        VkImageView _depthImageView;
    };

}

#endif /* !DEPTH_IMAGE_HPP */

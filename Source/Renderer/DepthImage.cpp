//
// Created by Timothée Couble on 10/12/2019.
//

#include "DepthImage.hpp"

void renderer::DepthImage::setUp(Devices &devices, VkExtent2D &extent) {
    VkFormat depthFormat = findDepthFormat(devices.getPhysical());

    BufferManip::createImage(devices, extent.width, extent.height, depthFormat,
                             VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _depthImage, _depthImageMemory);
    _depthImageView = BufferManip::createImageView(devices.get(), _depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void renderer::DepthImage::cleanUp(VkDevice &device) {
    vkDestroyImageView(device, _depthImageView, nullptr);
    vkDestroyImage(device, _depthImage, nullptr);
    vkFreeMemory(device, _depthImageMemory, nullptr);
}

VkImageView &renderer::DepthImage::get() {
    return _depthImageView;
}

//
// Created by Timothée Couble on 09/12/2019.
//

#ifndef BUFFER_MANIP_HPP
# define BUFFER_MANIP_HPP

#include "Devices.hpp"
#include "Utils.hpp"

namespace renderer {

    class BufferManip {
    public:
        static void createBuffer(Devices &devices, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                          VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        static void copyBuffer(Devices &devices, VkCommandPool &pool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        static void createImage(Devices &devices, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        static VkImageView createImageView(VkDevice &device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        static void copyBufferToImage(Devices &devices, VkCommandPool &pool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        static void transitionImageLayout(Devices &devices, VkCommandPool &pool, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

    private:
        static VkCommandBuffer beginSingleTimeCommands(VkDevice &device, VkCommandPool &pool);
        static void endSingleTimeCommands(Devices &devices, VkCommandPool &pool, VkCommandBuffer commandBuffer);

    };
}

#endif /* !BUFFER_MANIP_HPP */

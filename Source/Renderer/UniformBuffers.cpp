//
// Created by Timothée Couble on 10/12/2019.
//

#include "UniformBuffers.hpp"

void renderer::UniformBuffers::setUp(Devices &devices, size_t size) {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    _uniformBuffers.resize(size);
    _uniformBuffersMemory.resize(size);

    for (size_t i = 0; i < size; i++)
        BufferManip::createBuffer(devices, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                  _uniformBuffers[i], _uniformBuffersMemory[i]);
}

void renderer::UniformBuffers::cleanUp(VkDevice &device) {
    for (size_t i = 0; i < _uniformBuffers.size(); i++) {
        vkDestroyBuffer(device, _uniformBuffers[i], nullptr);
        vkFreeMemory(device, _uniformBuffersMemory[i], nullptr);
    }
}

VkBuffer &renderer::UniformBuffers::operator[](size_t i) {
    return _uniformBuffers[i];
}

VkDeviceMemory &renderer::UniformBuffers::getMemory(size_t i) {
    return _uniformBuffersMemory[i];
}

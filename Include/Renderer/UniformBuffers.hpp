//
// Created by Timothée Couble on 10/12/2019.
//

#ifndef UNIFORM_BUFFERS_HPP
# define UNIFORM_BUFFERS_HPP

#include <vector>
#include <array>

#include "BufferManip.hpp"
#include "Devices.hpp"
#include "Libraries.hpp"
#include "Utils.hpp"

namespace renderer {

    /**
     * Uniform buffer objects (UBO).
     */
    class UniformBuffers {
    public:
        UniformBuffers() = default;

        void setUp(Devices &devices, size_t size);
        void cleanUp(VkDevice &device);

        VkBuffer &operator[](size_t i);
        VkDeviceMemory &getMemory(size_t i);

    private:
        std::vector<VkBuffer> _uniformBuffers;
        std::vector<VkDeviceMemory> _uniformBuffersMemory;

    };

}

#endif /* !UNIFORM_BUFFERS_HPP */

//
// Created by Timothée Couble on 10/12/2019.
//

#ifndef COMMAND_POOL_HPP
# define COMMAND_POOL_HPP

#include "Libraries.hpp"
#include "Utils.hpp"

namespace renderer {

    /**
     * Command pools manage the memory that is used to store
     * the buffers and command buffers are allocated from them.
     */
    class CommandPool {
    public:
        CommandPool() = default;

        void setUp(VkDevice &device, QueueFamilyIndices queueFamilyIndices);
        void cleanUp(VkDevice &device);

        VkCommandPool &get();

    private:
        VkCommandPool _commandPool = nullptr;

    };

}


#endif /* !COMMAND_POOL_HPP */

//
// Created by Timothée Couble on 10/12/2019.
//

#ifndef SEMAPHORES_HPP
# define SEMAPHORES_HPP

#include <vector>

#include "Devices.hpp"
#include "Libraries.hpp"
#include "Model.hpp"
#include "SwapChain.hpp"
#include "Utils.hpp"

namespace renderer {

    /**
     * SyncObjects that can be used for coordinating operations
     * by having one operation signal and another operation
     * wait for a fence or semaphore to go from the unsignaled
     * to signaled state.
     *
     * Fences are mainly designed to synchronize your application
     * itself with rendering operation, whereas semaphores are used
     * to synchronize operations within or across command queues.
     */
    class SyncObjects {
    public:
        SyncObjects() = default;

        void setUp(VkDevice &device, size_t size);
        void cleanUp(VkDevice &device);

        bool drawFrame(Devices &devices, SwapChain &swapChain, Model &model, Model &model2, bool isResized);

    private:
        size_t _currentFrame = 0;
        std::vector<VkSemaphore> _imageAvailableSemaphores;
        std::vector<VkSemaphore> _renderFinishedSemaphores;
        std::vector<VkFence> _inFlightFences;
        std::vector<VkFence> _imagesInFlight;
    };

}

#endif /* !SEMAPHORES_HPP */

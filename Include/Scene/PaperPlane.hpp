//
// Created by Timothée Couble on 04/02/2020.
//

#ifndef PAPER_PLANE_HPP
# define PAPER_PLANE_HPP

#include <algorithm>
#include <cmath>
#include <chrono>

#include "Model.hpp"
#include "Utils.hpp"

namespace scene {

    class PaperPlane : public renderer::Model {
    public:
        explicit PaperPlane(renderer::ModelColor color);
        ~PaperPlane() override = default;

        void update(renderer::Models_t &models) override;
        void updateUniformBuffer(VkDevice &device, uint32_t currentImage) override;

    private:
        glm::vec3 boundaries();

    private:
        const double _separationMin = 1.5f;
        const double _maxSpeed = 0.5f;
    };
}

std::ostream	&operator<<(std::ostream &stream, const glm::vec3 &vec);

#endif /* !PAPER_PLANE_HPP */

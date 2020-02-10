//
// Created by Timothée Couble on 05/02/2020.
//

#ifndef OBSTACLE_HPP
# define OBSTACLE_HPP

#include "Model.hpp"
#include "Utils.hpp"

namespace scene {

    class Cylinder : public renderer::Model {
    public:
        explicit Cylinder(renderer::ModelColor color);
        ~Cylinder() override = default;

        void update(renderer::Models_t &models) override;
        void updateUniformBuffer(VkDevice &device, uint32_t currentImage) override;
        bool willCollide(glm::vec3 position) override;

    private:
        glm::vec3 _size = glm::vec3(6.0f, 10.0f, 6.0f);
        const double _separationOffset = 20.0f;

    };
}


#endif /* !OBSTACLE_HPP */

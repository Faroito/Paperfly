//
// Created by Timothée Couble on 10/12/2019.
//

#ifndef PAPERFLY_HPP
# define PAPERFLY_HPP

#include <iostream>

#include "Application.hpp"

namespace scene {
    class Paperfly : public renderer::Application {
    public:
        Paperfly();

        void onDraw() override;

    private:


    };
}

#endif /* !PAPERFLY_HPP */

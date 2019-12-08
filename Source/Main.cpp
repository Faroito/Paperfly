//
// Created by Timothée Couble on 08/12/2019.
//

#include "Application.hpp"
#include "Library.hpp"

int main(int ac, char *av[]) {
    Application app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
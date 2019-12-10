//
// Created by Timothée Couble on 10/12/2019.
//

#ifndef WINDOW_HPP
# define WINDOW_HPP

#include "IApplication.hpp"
#include "Libraries.hpp"
#include "Utils.hpp"

namespace renderer {

    /**
     * GLFWWindow class abstraction.
     */
    class Window {
    public:
        Window() = default;

        void setUp(IApplication *app, const std::string &appName);
        void cleanUp();

        GLFWwindow *get();
        bool shouldClose();

        void resize();
        void close();

    private:
        static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    public:
        bool resized = false;

    private:
        GLFWwindow *_window = nullptr;
    };

}

#endif /* !WINDOW_HPP */

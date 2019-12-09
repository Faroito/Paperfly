//
// Created by Timothée Couble on 09/12/2019.
//

#ifndef INSTANCE_HPP
# define INSTANCE_HPP

#include <vector>

#include "Debug.hpp"
#include "Libraries.hpp"
#include "Utils.hpp"

namespace renderer {

    /**
     * The instance is the connection between the application and
     * the Vulkan library and creating it involves specifying some
     * details about your application to the driver.
     */
    class Instance {
    public:
        Instance() = default;

        void setUp(const std::string &name);
        void cleanUp();

        VkInstance &get();

    private:
        static bool checkValidationLayerSupport();
        static std::vector<const char*> getRequiredExtensions();

    private:
        VkInstance _instance;

    };

}

#endif /* !INSTANCE_HPP */

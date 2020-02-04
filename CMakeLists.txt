project(Paperfly)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17")

add_executable(${PROJECT_NAME}
        Source/Renderer/Application.cpp
        Source/Renderer/BufferManip.cpp
        Source/Renderer/CommandBuffers.cpp
        Source/Renderer/CommandPool.cpp
        Source/Renderer/Debug.cpp
        Source/Renderer/DepthImage.cpp
        Source/Renderer/DescriptorSets.cpp
        Source/Renderer/Devices.cpp
        Source/Renderer/Framebuffers.cpp
        Source/Renderer/GraphicsPipeline.cpp
        Source/Renderer/Instance.cpp
        Source/Renderer/Mesh.cpp
        Source/Renderer/Model.cpp
        Source/Renderer/Surface.cpp
        Source/Renderer/SwapChain.cpp
        Source/Renderer/SyncObjects.cpp
        Source/Renderer/Texture.cpp
        Source/Renderer/UniformBuffers.cpp
        Source/Renderer/Utils.cpp
        Source/Renderer/Window.cpp
        Source/Scene/Camera.cpp
        Source/Scene/Paperfly.cpp
        Source/Main.cpp)

find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/Include/)
include_directories(${PROJECT_SOURCE_DIR}/Include/Renderer/)
include_directories(${PROJECT_SOURCE_DIR}/Include/Scene/)
include_directories(${PROJECT_SOURCE_DIR}/Libraries/)

if (VULKAN_FOUND)
    message(STATUS "Found Vulkan, Including and Linking now")
    include_directories(${Vulkan_INCLUDE_DIRS})
    target_link_libraries (${PROJECT_NAME} ${Vulkan_LIBRARIES} glfw)
endif (VULKAN_FOUND)

project(Paperfly)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17")

add_executable(${PROJECT_NAME}
        Source/Renderer/Debug.cpp
        Source/Renderer/Utils.cpp
        Source/Application.cpp
        Source/Main.cpp)

find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/Include/)
include_directories(${PROJECT_SOURCE_DIR}/Include/Renderer/)
include_directories(${PROJECT_SOURCE_DIR}/Libraries/)

if (VULKAN_FOUND)
    message(STATUS "Found Vulkan, Including and Linking now")
    include_directories(${Vulkan_INCLUDE_DIRS})
    target_link_libraries (${PROJECT_NAME} ${Vulkan_LIBRARIES} glfw)
endif (VULKAN_FOUND)

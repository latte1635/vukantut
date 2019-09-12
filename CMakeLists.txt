cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(VulkanTutorial)
find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

file(GLOB SOURCES "src/*.cpp")

include_directories("$ENV{VULKAN_SDK}/include")
link_directories("$ENV{VULKAN_SDK}/lib")
link_directories("$ENV{VULKAN_SDK}/etc/explicit_layer.d")


include_directories("/home/latte/code/libs/glfw-3.3/include")

add_executable(VulkanTutorial VulkanTutorial.cpp)

target_link_libraries(VulkanTutorial vulkan glfw)
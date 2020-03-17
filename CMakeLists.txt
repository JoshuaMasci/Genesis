cmake_minimum_required(VERSION 3.8.0)
project(Genesis)

add_subdirectory(Genesis)

add_subdirectory(Platforms/SDL2)
add_subdirectory(Platforms/Vulkan)

add_subdirectory(Sandbox)

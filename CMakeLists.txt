cmake_minimum_required(VERSION 3.16.0)
project(Genesis)

option (INCLUDE_EASY_PROFILER "Includes Profiling tool" OFF)

add_subdirectory(Genesis)

add_subdirectory(Platforms/SDL2)
add_subdirectory(Platforms/Opengl)
#add_subdirectory(Platforms/Vulkan)

add_subdirectory(Genesis_Editor)

add_subdirectory(Sandbox)

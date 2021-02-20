cmake_minimum_required(VERSION 3.16.0)
project(Genesis)

option (INCLUDE_EASY_PROFILER "Includes Profiling tool" OFF)

add_subdirectory(core)

add_subdirectory(platforms/SDL2)
add_subdirectory(platforms/Opengl)
add_subdirectory(platforms/vulkan)

add_subdirectory(engine)
add_subdirectory(editor)
add_subdirectory(sandbox)

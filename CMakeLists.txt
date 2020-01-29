cmake_minimum_required(VERSION 3.8.0)
project(Genesis_Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option (BUILD_SANDBOX "Creates the Sandbox app" ON) 

option (INCLUDE_SDL2 "Includes the SDL2 Platform Interface" ON)
option (INCLUDE_VULKAN "Includes the Vulkan Graphics API" ON)
option (INCLUDE_EASY_PROFILER "Includes Profiling tool" ON)

add_definitions(-DGENESIS_PLATFORM_WIN)

add_subdirectory(Genesis)

add_subdirectory(Platforms)

if(BUILD_SANDBOX)
	add_subdirectory(Sandbox)
endif()
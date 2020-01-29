cmake_minimum_required(VERSION 3.8.0)

include_directories(${CMAKE_SOURCE_DIR}/lib/glm/)
include_directories(${CMAKE_SOURCE_DIR}/lib/concurrent_queue/)
include_directories(${CMAKE_SOURCE_DIR}/lib/spdlog-1.5.0/include)

if(INCLUDE_SDL2)
	add_subdirectory(SDL2)
endif()

if(INCLUDE_VULKAN)
	add_subdirectory(Vulkan)
endif()
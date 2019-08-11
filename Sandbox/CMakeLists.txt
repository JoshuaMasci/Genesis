cmake_minimum_required(VERSION 3.8.0)
project(Sandbox)

include_directories(src)
file(GLOB_RECURSE SANDBOX_SOURCES "src/Sandbox/*.cpp")
file(GLOB_RECURSE SANDBOX_HEADERS "src/Sandbox/*.hpp")
source_group("Sandbox" FILES ${SANDBOX_SOURCES})
source_group("Sandbox_Headers" FILES ${SANDBOX_HEADERS})

include_directories(${Genesis_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/submodules/glm/)
include_directories(${CMAKE_SOURCE_DIR}/submodules/entt/src/)
include_directories(${CMAKE_SOURCE_DIR}/submodules/concurrent_queue/)
include_directories(${CMAKE_SOURCE_DIR}/submodules/stb/)
include_directories(${CMAKE_SOURCE_DIR}/submodules/tinyobj/)

link_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/)
link_directories(${CMAKE_SOURCE_DIR}/submodules/bullet_physics/lib/)

#OPENGL
#include_directories(${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/include/)
#link_directories(${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/lib/Release/x64/)

if(INCLUDE_VULKAN)
	find_package(Vulkan REQUIRED)
	include_directories(${CMAKE_SOURCE_DIR}/submodules/vulkan_memory_allocator/src/)
	include_directories(Vulkan_INCLUDE_DIR)
endif()

add_executable(Sandbox ${SANDBOX_SOURCES} ${SANDBOX_HEADERS})
set_target_properties(Sandbox PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${Sandbox_SOURCE_DIR}")

target_link_libraries(Sandbox Genesis)


cmake_minimum_required(VERSION 3.8.0)
project(Sandbox)

include_directories(src)

file(GLOB_RECURSE SANDBOX_SOURCES "src/Sandbox/*.cpp")
file(GLOB_RECURSE SANDBOX_HEADERS "src/Sandbox/*.hpp")
source_group("Sandbox" FILES ${SANDBOX_SOURCES})
source_group("Sandbox_Headers" FILES ${SANDBOX_HEADERS})

include_directories(${Genesis_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/lib/glm/)
include_directories(${CMAKE_SOURCE_DIR}/lib/concurrent_queue/)
include_directories(${CMAKE_SOURCE_DIR}/lib/stb/)
include_directories(${CMAKE_SOURCE_DIR}/lib/tinyobj/)
include_directories(${CMAKE_SOURCE_DIR}/lib/imgui/)

link_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/)

#OPENGL
#include_directories(${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/include/)
#link_directories(${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/lib/Release/x64/)

link_directories(${CMAKE_SOURCE_DIR}/lib/assimp-3.3.1/build/code/Debug/)
link_directories(${CMAKE_SOURCE_DIR}/lib/assimp-3.3.1/build/contrib/zlib/Debug/)

if(INCLUDE_VULKAN)
	find_package(Vulkan REQUIRED FATAL_ERROR)
	include_directories(${Vulkan_INCLUDE_DIR})	
	include_directories(${CMAKE_SOURCE_DIR}/lib/vulkan_memory_allocator/src/)
	include_directories(${CMAKE_SOURCE_DIR}/lib/spirv-reflect/)
endif()

add_executable(Sandbox ${SANDBOX_SOURCES} ${SANDBOX_HEADERS})
set_target_properties(Sandbox PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${Sandbox_SOURCE_DIR}")

target_link_libraries(Sandbox Genesis)


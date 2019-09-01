cmake_minimum_required(VERSION 3.8.0)
project(Genesis)

include_directories(src)

file(GLOB GENESIS_MAIN "src/Genesis/*.*")
source_group("Main" FILES ${GENESIS_MAIN})

file(GLOB_RECURSE GENESIS_CORE "src/Genesis/Core/*.*")
source_group("Core" FILES ${GENESIS_CORE})

#TODO Select Subdirectories
file(GLOB GENESIS_RENDERING "src/Genesis/Rendering/*.*")
source_group("Rendering Core" FILES ${GENESIS_RENDERING})

#file(GLOB_RECURSE GENESIS_RENDERING_OPENGL "src/Genesis/Rendering/OpenGL/*.*")
#source_group("Rendering OpenGL" FILES ${GENESIS_RENDERING_OPENGL})

file(GLOB_RECURSE GENESIS_RENDERING_VULKAN "src/Genesis/Rendering/Vulkan/*.*")
source_group("Rendering Vulkan" FILES ${GENESIS_RENDERING_VULKAN})

file(GLOB_RECURSE GENESIS_INPUT "src/Genesis/Input/*.*")
source_group("Input" FILES ${GENESIS_INPUT})

file(GLOB_RECURSE GENESIS_JOB "src/Genesis/Job/*.*")
source_group("Job" FILES ${GENESIS_JOB})

if(INCLUDE_VULKAN)
	file(GLOB GENESIS_SPIRV "${CMAKE_SOURCE_DIR}/submodules/spirv-reflect/spirv_reflect.c" "${CMAKE_SOURCE_DIR}/submodules/spirv-reflect/spirv_reflect.h")
	source_group("SPIRV UTILS" FILES ${GENESIS_SPIRV})
endif()

#TODO Select Subdirectories
file(GLOB_RECURSE GENESIS_PLATFORM "src/Genesis/Platform/*.*")
source_group("Platform" FILES ${GENESIS_PLATFORM})

file(GLOB_RECURSE GENESIS_PHYSICS "src/Genesis/Physics/*.*")
source_group("Physics" FILES ${GENESIS_PHYSICS})

#Create Libary
add_library(Genesis STATIC ${GENESIS_MAIN} ${GENESIS_CORE} ${GENESIS_RENDERING} ${GENESIS_RENDERING_VULKAN} ${GENESIS_INPUT} ${GENESIS_JOB} ${GENESIS_PHYSICS} ${GENESIS_PLATFORM} ${GENESIS_SPIRV})

#GLM
include_directories(${CMAKE_SOURCE_DIR}/submodules/glm/)

#SDL2
include_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/include/)
link_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/)
target_link_libraries(Genesis PRIVATE SDL2)

#ENTT
include_directories(${CMAKE_SOURCE_DIR}/submodules/entt/src/)

#Concurrent Queue
include_directories(${CMAKE_SOURCE_DIR}/submodules/concurrent_queue/)

#OPENGL
#include_directories(${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/include/)
#link_directories(${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/lib/Release/x64/)
#target_link_libraries (Genesis PRIVATE glew32)
#target_link_libraries (Genesis PRIVATE opengl32)
#target_link_libraries (Genesis PRIVATE glu32)


if(INCLUDE_VULKAN)
	find_package(Vulkan REQUIRED)
	include_directories(Vulkan_INCLUDE_DIR)
	target_link_libraries(Genesis PRIVATE Vulkan::Vulkan)
	
	include_directories(${CMAKE_SOURCE_DIR}/submodules/vulkan_memory_allocator/src/)
	include_directories(${CMAKE_SOURCE_DIR}/submodules/spirv-reflect/)
endif()

#STB
include_directories(${CMAKE_SOURCE_DIR}/submodules/stb/)

#TINYOBJ
include_directories(${CMAKE_SOURCE_DIR}/submodules/tinyobj/)

#ImGUI
include_directories(${CMAKE_SOURCE_DIR}/submodules/imgui/)


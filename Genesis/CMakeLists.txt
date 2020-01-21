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
	file(GLOB GENESIS_SPIRV "${CMAKE_SOURCE_DIR}/lib/spirv-reflect/spirv_reflect.c" "${CMAKE_SOURCE_DIR}/lib/spirv-reflect/spirv_reflect.h")
	source_group("SPIRV UTILS" FILES ${GENESIS_SPIRV})
endif()

#TODO Select Subdirectories
file(GLOB_RECURSE GENESIS_PLATFORM "src/Genesis/Platform/*.*")
source_group("Platform" FILES ${GENESIS_PLATFORM})

file(GLOB_RECURSE GENESIS_PHYSICS "src/Genesis/Physics/*.*")
source_group("Physics" FILES ${GENESIS_PHYSICS})

#Create Libary
add_library(Genesis STATIC ${GENESIS_MAIN} ${GENESIS_CORE} ${GENESIS_RENDERING} ${GENESIS_RENDERING_VULKAN} ${GENESIS_INPUT} ${GENESIS_JOB} ${GENESIS_PHYSICS} ${GENESIS_PLATFORM} ${GENESIS_SPIRV} ${GENESIS_GLTF})

include_directories(${CMAKE_SOURCE_DIR}/lib/glm/)
include_directories(${CMAKE_SOURCE_DIR}/lib/concurrent_queue/)
include_directories(${CMAKE_SOURCE_DIR}/lib/stb/)
include_directories(${CMAKE_SOURCE_DIR}/lib/tinyobj/)
include_directories(${CMAKE_SOURCE_DIR}/lib/imgui/)
include_directories(${CMAKE_SOURCE_DIR}/lib/spdlog-1.5.0/include)

#SDL2
include_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/include/)
link_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/)
target_link_libraries(Genesis PRIVATE SDL2)

#TEMP ASSIMP
link_directories(${CMAKE_SOURCE_DIR}/lib/assimp-3.3.1/build/code/Debug/)
link_directories(${CMAKE_SOURCE_DIR}/lib/assimp-3.3.1/build/contrib/zlib/Debug/)
include_directories(${CMAKE_SOURCE_DIR}/lib/assimp-3.3.1/include/)
target_link_libraries (Genesis PRIVATE zlibstaticd)
target_link_libraries (Genesis PRIVATE assimp-vc140-mt)

#TEMP ENTT
include_directories(${CMAKE_SOURCE_DIR}/lib/entt-3.2.2/src/)

if(INCLUDE_VULKAN)
	find_package(Vulkan REQUIRED FATAL_ERROR)
	include_directories(${Vulkan_INCLUDE_DIR})	
	target_link_libraries(Genesis PRIVATE Vulkan::Vulkan)
	
	include_directories(${CMAKE_SOURCE_DIR}/lib/vulkan_memory_allocator/src/)
	include_directories(${CMAKE_SOURCE_DIR}/lib/spirv-reflect/)
endif()

if(INCLUDE_EASY_PROFILER)
	include_directories(${CMAKE_SOURCE_DIR}/lib/easy_profiler-v2.1.0-msvc15-win64/include/)
	link_directories(${CMAKE_SOURCE_DIR}/lib/easy_profiler-v2.1.0-msvc15-win64/lib)
	target_link_libraries(Genesis PRIVATE easy_profiler)
	add_definitions(-DGENESIS_PROFILER_ENABLED)
endif()
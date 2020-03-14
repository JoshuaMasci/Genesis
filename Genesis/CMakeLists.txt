cmake_minimum_required(VERSION 3.8.0)
project(Genesis)

include_directories(${Genesis_SOURCE_DIR})

file(GLOB GENESIS_MAIN "Genesis/*.*")
source_group("Main" FILES ${GENESIS_MAIN})

file(GLOB_RECURSE GENESIS_CORE "Genesis/Core/*.*")
source_group("Core" FILES ${GENESIS_CORE})

file(GLOB_RECURSE GENESIS_DEBUG "Genesis/Debug/*.*")
source_group("Debug" FILES ${GENESIS_DEBUG})

file(GLOB_RECURSE GENESIS_ECS "Genesis/Ecs/*.*")
source_group("ECS" FILES ${GENESIS_ECS})

file(GLOB_RECURSE GENESIS_ENTITY "Genesis/Entity/*.*")
source_group("Entity" FILES ${GENESIS_ENTITY})

file(GLOB_RECURSE GENESIS_INPUT "Genesis/Input/*.*")
source_group("Input" FILES ${GENESIS_INPUT})

file(GLOB_RECURSE GENESIS_JOB "Genesis/Job/*.*")
source_group("Job" FILES ${GENESIS_JOB})

file(GLOB_RECURSE GENESIS_PHYSICS "Genesis/Physics/*.*")
source_group("Physics" FILES ${GENESIS_PHYSICS})

file(GLOB_RECURSE GENESIS_PLATFORM "Genesis/Platform/*.*")
source_group("Platform" FILES ${GENESIS_PLATFORM})

file(GLOB GENESIS_RENDERING "Genesis/Rendering/*.*")
source_group("Rendering" FILES ${GENESIS_RENDERING})

file(GLOB GENESIS_SCENE "Genesis/Scene/*.*")
source_group("Scene" FILES ${GENESIS_SCENE})

#Create Libary
add_library(Genesis STATIC ${GENESIS_MAIN} ${GENESIS_CORE} ${GENESIS_DEBUG} ${GENESIS_ECS} ${GENESIS_ENTITY} ${GENESIS_INPUT} ${GENESIS_JOB} ${GENESIS_PHYSICS} ${GENESIS_PLATFORM} ${GENESIS_RENDERING} ${GENESIS_SCENE})

include_directories(${CMAKE_SOURCE_DIR}/lib/glm/)
include_directories(${CMAKE_SOURCE_DIR}/lib/concurrent_queue/)
include_directories(${CMAKE_SOURCE_DIR}/lib/stb/)
include_directories(${CMAKE_SOURCE_DIR}/lib/tinyobj/)
include_directories(${CMAKE_SOURCE_DIR}/lib/imgui/)
include_directories(${CMAKE_SOURCE_DIR}/lib/spdlog-1.5.0/include)

#TEMP ASSIMP
link_directories(${CMAKE_SOURCE_DIR}/lib/assimp-3.3.1/build/code/Debug/)
link_directories(${CMAKE_SOURCE_DIR}/lib/assimp-3.3.1/build/contrib/zlib/Debug/)
include_directories(${CMAKE_SOURCE_DIR}/lib/assimp-3.3.1/include/)
target_link_libraries (Genesis zlibstaticd)
target_link_libraries (Genesis assimp-vc140-mt)

#ENTT
include_directories(${CMAKE_SOURCE_DIR}/lib/entt-3.3.1/src/)

#ReactPhysics3D
include_directories(${CMAKE_SOURCE_DIR}/lib/reactphysics3d-master/src/)
link_directories(${CMAKE_SOURCE_DIR}/lib/reactphysics3d-master/build/lib/)
target_link_libraries (Genesis reactphysics3d)
add_definitions(-DIS_DOUBLE_PRECISION_ENABLED)

if(INCLUDE_EASY_PROFILER)
	include_directories(${CMAKE_SOURCE_DIR}/lib/easy_profiler-v2.1.0-msvc15-win64/include/)
	link_directories(${CMAKE_SOURCE_DIR}/lib/easy_profiler-v2.1.0-msvc15-win64/lib)
	target_link_libraries(Genesis easy_profiler)
	add_definitions(-DGENESIS_PROFILER_ENABLED)
endif()

set_target_properties(Genesis
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
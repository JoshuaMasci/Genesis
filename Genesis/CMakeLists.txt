cmake_minimum_required(VERSION 3.8.0)
project(Genesis)

include_directories(src)


file(GLOB GENESIS_MAIN "src/Genesis/*.*")
source_group("Main" FILES ${GENESIS_MAIN})

file(GLOB_RECURSE GENESIS_CORE "src/Genesis/Core/*.*")
source_group("Core" FILES ${GENESIS_CORE})

#TODO Select Subdirectories
file(GLOB_RECURSE GENESIS_GRAPHICS "src/Genesis/Graphics/*.*")
source_group("Graphics" FILES ${GENESIS_GRAPHICS})

file(GLOB_RECURSE GENESIS_INPUT "src/Genesis/Input/*.*")
source_group("Input" FILES ${GENESIS_INPUT})

file(GLOB_RECURSE GENESIS_JOB "src/Genesis/Job/*.*")
source_group("Job" FILES ${GENESIS_JOB})

file(GLOB_RECURSE GENESIS_PHYSICS "src/Genesis/Physics/*.*")
source_group("Physics" FILES ${GENESIS_PHYSICS})

#TODO Select Subdirectories
file(GLOB_RECURSE GENESIS_PLATFORM "src/Genesis/Platform/*.*")
source_group("Platform" FILES ${GENESIS_PLATFORM})

#Create Libary
add_library(Genesis STATIC ${GENESIS_MAIN} ${GENESIS_CORE} ${GENESIS_GRAPHICS} ${GENESIS_INPUT} ${GENESIS_JOB} ${GENESIS_PHYSICS} ${GENESIS_PLATFORM})

#GLM
include_directories(${CMAKE_SOURCE_DIR}/lib/glm/)

#SDL2
include_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/include/)
link_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/)
target_link_libraries(Genesis PRIVATE SDL2)

#Bullet
include_directories(${CMAKE_SOURCE_DIR}/lib/bullet_physics/src/)
link_directories(${CMAKE_SOURCE_DIR}/lib/bullet_physics/lib/)
target_link_libraries(Genesis PRIVATE debug BulletCollision_Debug optimized BulletCollision)
target_link_libraries(Genesis PRIVATE debug BulletDynamics_Debug optimized BulletDynamics)
target_link_libraries(Genesis PRIVATE debug LinearMath_Debug optimized LinearMath)
target_compile_definitions(Genesis PUBLIC BT_USE_DOUBLE_PRECISION)

#ENTT
include_directories(${CMAKE_SOURCE_DIR}/lib/entt/src/)

#Concurrent Queue
include_directories(${CMAKE_SOURCE_DIR}/lib/concurrent_queue/)

#OPENGL
include_directories(${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/include/)
link_directories(${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/lib/Release/x64/)
target_link_libraries (Genesis PRIVATE glew32)
target_link_libraries (Genesis PRIVATE opengl32)
target_link_libraries (Genesis PRIVATE glu32)

#STB
include_directories(${CMAKE_SOURCE_DIR}/lib/stb/)

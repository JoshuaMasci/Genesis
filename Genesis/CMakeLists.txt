cmake_minimum_required(VERSION 3.8.0)
project(Genesis)

include_directories(src)
file(GLOB_RECURSE GENESIS_SOURCES "src/Genesis/*.cpp")
file(GLOB_RECURSE GENESIS_HEADERS "src/Genesis/*.hpp")
source_group("Genesis" FILES ${GENESIS_SOURCES})
source_group("Genesis_Headers" FILES ${GENESIS_HEADERS})

#Create Libary
add_library(Genesis STATIC ${GENESIS_SOURCES} ${GENESIS_HEADERS})

#GLM
include_directories(${CMAKE_SOURCE_DIR}/lib/glm/)

#OPENGL
target_link_libraries (Genesis PRIVATE opengl32)
target_link_libraries (Genesis PRIVATE glu32)

#SDL2
link_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/)
include_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/include/)
target_link_libraries (Genesis PRIVATE SDL2)

#Glew
link_directories(${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/lib/Release/x64/)
include_directories(${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/include/)
target_link_libraries (Genesis PRIVATE glew32)

#Bullet
link_directories(${CMAKE_SOURCE_DIR}/lib/bullet3-2.86.1/build/lib/)

include_directories(${CMAKE_SOURCE_DIR}/lib/bullet3-2.86.1/src/)
target_link_libraries  (Genesis PRIVATE debug BulletCollision_Debug optimized BulletCollision)
target_link_libraries  (Genesis PRIVATE debug BulletDynamics_Debug optimized BulletDynamics)
target_link_libraries  (Genesis PRIVATE debug LinearMath_Debug optimized LinearMath)
target_compile_definitions(Genesis PUBLIC BT_USE_DOUBLE_PRECISION)

#ENTT
include_directories(${CMAKE_SOURCE_DIR}/lib/entt-cpp14/src/)

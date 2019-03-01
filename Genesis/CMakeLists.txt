cmake_minimum_required(VERSION 3.8.0)
project(Genesis)

include_directories(src)
file(GLOB_RECURSE GENESIS_SOURCES "src/Genesis/*.cpp")
file(GLOB_RECURSE GENESIS_HEADERS "src/Genesis/*.hpp")
source_group("Genesis" FILES ${GENESIS_SOURCES})
source_group("Genesis_Headers" FILES ${GENESIS_HEADERS})

link_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.4/lib/x64/)
link_directories(${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/lib/Release/x64/)
link_directories(${CMAKE_SOURCE_DIR}/lib/bullet3-2.86.1/build/lib/Debug/)

#Create Libary
add_library(Genesis STATIC ${GENESIS_SOURCES} ${GENESIS_HEADERS})

#GLM
include_directories(${CMAKE_SOURCE_DIR}/lib/glm/)

#OPENGL
target_link_libraries (Genesis PRIVATE opengl32)
target_link_libraries (Genesis PRIVATE glu32)

#SDL2
include_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.4/include/)
target_link_libraries (Genesis PRIVATE SDL2)

#Glew
include_directories(${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/include/)
target_link_libraries (Genesis PRIVATE glew32)

#Bullet
include_directories(${CMAKE_SOURCE_DIR}/lib/bullet3-2.86.1/src/)
target_link_libraries  (Genesis PRIVATE BulletCollision_Debug)
target_link_libraries  (Genesis PRIVATE BulletDynamics_Debug)
target_link_libraries  (Genesis PRIVATE LinearMath_Debug)
target_compile_definitions(Genesis PUBLIC BT_USE_DOUBLE_PRECISION)

#spdlog
include_directories(${CMAKE_SOURCE_DIR}/lib/spdlog-1.2.1/include)

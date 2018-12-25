cmake_minimum_required(VERSION 3.8.0)
project(Genesis)

include_directories(src)
file(GLOB_RECURSE GENESIS_SOURCES "src/Genesis/*.cpp")
file(GLOB_RECURSE GENESIS_HEADERS "src/Genesis/*.hpp")
source_group("Genesis" FILES ${GENESIS_SOURCES})
source_group("Genesis_Headers" FILES ${GENESIS_HEADERS})

link_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.4/lib/x64/)
link_directories(${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/lib/Release/x64/)
link_directories(${CMAKE_SOURCE_DIR}/lib/reactphysics3d-master/build/lib/Debug/)

#Create Libary
add_library(Genesis SHARED ${GENESIS_SOURCES} ${GENESIS_HEADERS})
target_compile_definitions(Genesis PRIVATE GENESIS_BUILD_DLL)

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

#REACT_PHYSICS_3D
include_directories(${CMAKE_SOURCE_DIR}/lib/reactphysics3d-master/src)
target_link_libraries (Genesis PRIVATE reactphysics3d)
target_compile_definitions(Genesis PUBLIC IS_DOUBLE_PRECISION_ENABLED)

if(BUILD_SANDBOX)
	#POST BUILD COPY, ONLY DEBUG RIGHT NOW
	add_custom_command(TARGET Genesis POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${Genesis_BINARY_DIR}/Debug/${PROJECT_NAME}.dll"  "${Sandbox_BINARY_DIR}/Debug/${PROJECT_NAME}.dll")
endif()

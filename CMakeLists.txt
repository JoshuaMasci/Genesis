cmake_minimum_required(VERSION 3.8.0)
project(Genesis)

option (BUILD_SANDBOX "Creates the Sandbox app" ON) 

include_directories(src)
file(GLOB_RECURSE GENESIS_SOURCES "src/Genesis/*.cpp")
file(GLOB_RECURSE GENESIS_HEADERS "src/Genesis/*.hpp")
source_group("Genesis" FILES ${GENESIS_SOURCES})
source_group("Genesis_Headers" FILES ${GENESIS_HEADERS})

link_directories(lib/SDL2-2.0.4/lib/x64/)
link_directories(lib/glew-1.12.0/lib/Release/x64/)
link_directories(lib/reactphysics3d-master/build/lib/Debug/)

#Create Libary
add_library(Genesis SHARED ${GENESIS_SOURCES} ${GENESIS_HEADERS})
set_target_properties(Genesis PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

#GLM
include_directories(lib/glm/)

#OPENGL
target_link_libraries (Genesis opengl32)
target_link_libraries (Genesis glu32)

#SDL2
include_directories(lib/SDL2-2.0.4/include/)
target_link_libraries (Genesis SDL2)

#Glew
include_directories(lib/glew-1.12.0/include/)
target_link_libraries (Genesis glew32)

#REACT_PHYSICS_3D
include_directories(lib/reactphysics3d-master/src)
target_link_libraries (Genesis reactphysics3d)
target_compile_definitions(Genesis PUBLIC IS_DOUBLE_PRECISION_ENABLED)

if(BUILD_SANDBOX)
	project(Sandbox)
	include_directories(src)	
	file(GLOB_RECURSE SANDBOX_SOURCES "src/Sandbox/*.cpp")
	file(GLOB_RECURSE SANDBOX_HEADERS "src/Sandbox/*.hpp")
	
	add_executable(Sandbox ${SANDBOX_SOURCES} ${SANDBOX_HEADERS})
	
	target_link_libraries(Sandbox Genesis)
endif()
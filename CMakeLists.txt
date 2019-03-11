cmake_minimum_required(VERSION 3.8.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option (BUILD_SANDBOX "Creates the Sandbox app" ON) 

add_subdirectory(Genesis)

if(BUILD_SANDBOX)
	add_subdirectory(Sandbox)
endif()
cmake_minimum_required(VERSION 3.8.0)

option (BUILD_SANDBOX "Creates the Sandbox app" ON) 

add_subdirectory(Genesis)

if(BUILD_SANDBOX)
	add_subdirectory(Sandbox)
endif()
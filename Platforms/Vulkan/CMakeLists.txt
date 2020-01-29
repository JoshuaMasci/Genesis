cmake_minimum_required(VERSION 3.8.0)
project(Vulkan_Rendering)

include_directories(${Genesis_SOURCE_DIR})

file(GLOB VULKAN_SOURCES "*")
source_group("Rendering Vulkan" FILES ${VULKAN_SOURCES})
file(GLOB SPIRV "${CMAKE_SOURCE_DIR}/lib/spirv-reflect/spirv_reflect.c" "${CMAKE_SOURCE_DIR}/lib/spirv-reflect/spirv_reflect.h")

#Create Libary
add_library(Vulkan_Rendering STATIC ${VULKAN_SOURCES} ${SPIRV})

find_package(Vulkan REQUIRED FATAL_ERROR)
include_directories(${Vulkan_INCLUDE_DIR})	
target_link_libraries(Vulkan_Rendering Vulkan::Vulkan)

include_directories(${CMAKE_SOURCE_DIR}/lib/vulkan_memory_allocator/src/)
include_directories(${CMAKE_SOURCE_DIR}/lib/spirv-reflect/)

set_target_properties(Vulkan_Rendering
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

function(addVulkan TARGET)
	find_package(Vulkan REQUIRED FATAL_ERROR)
	include_directories(${Vulkan_INCLUDE_DIR})	
	include_directories(${CMAKE_SOURCE_DIR}/lib/vulkan_memory_allocator/src/)
	include_directories(${CMAKE_SOURCE_DIR}/lib/spirv-reflect/)
	include_directories(${CMAKE_SOURCE_DIR}/Platforms/Vulkan/)
	target_link_libraries(${TARGET} Vulkan_Rendering)
endfunction()
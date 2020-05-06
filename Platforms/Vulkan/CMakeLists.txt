cmake_minimum_required(VERSION 3.16.0)
project(Vulkan_Rendering CXX)

file(GLOB_RECURSE VULKAN_SOURCES "source/*.*")
file(GLOB_RECURSE VULKAN_HEADERS "include/*.*")


add_library(Vulkan_Rendering STATIC ${VULKAN_SOURCES} ${VULKAN_HEADERS} "${CMAKE_SOURCE_DIR}/lib/spirv-reflect/spirv_reflect.c" "${CMAKE_SOURCE_DIR}/lib/spirv-reflect/spirv_reflect.h")

target_include_directories(Vulkan_Rendering PUBLIC include/)

target_compile_features(Vulkan_Rendering INTERFACE cxx_std_17)
#target_compile_features(Vulkan_Rendering PUBLIC cxx_auto_type cxx_constexpr)

target_link_libraries(Vulkan_Rendering PUBLIC Genesis_Engine)

find_package(Vulkan REQUIRED FATAL_ERROR)

target_include_directories(Vulkan_Rendering PUBLIC ${Vulkan_INCLUDE_DIR})
target_include_directories(Vulkan_Rendering PUBLIC ${CMAKE_SOURCE_DIR}/lib/vulkan_memory_allocator/src/)
target_include_directories(Vulkan_Rendering PUBLIC ${CMAKE_SOURCE_DIR}/lib/spirv-reflect/)
target_link_libraries(Vulkan_Rendering PRIVATE Vulkan::Vulkan)

set_target_properties(Vulkan_Rendering
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
cmake_minimum_required(VERSION 3.16.0)
project(VulkanRenderer CXX)

file(GLOB_RECURSE VULKAN_SOURCES "source/*.*")
file(GLOB_RECURSE VULKAN_HEADERS "include/*.*")

add_library(VulkanRenderer STATIC ${VULKAN_SOURCES} ${VULKAN_HEADERS})
target_include_directories(VulkanRenderer PUBLIC include/)
target_compile_features(VulkanRenderer INTERFACE cxx_std_17)

find_package(Vulkan REQUIRED FATAL_ERROR)
target_include_directories(VulkanRenderer PUBLIC ${Vulkan_INCLUDE_DIR})
target_include_directories(VulkanRenderer PUBLIC ${CMAKE_SOURCE_DIR}/submodules/VulkanMemoryAllocator/src/)
target_link_libraries(VulkanRenderer PRIVATE Vulkan::Vulkan)

set_target_properties(VulkanRenderer
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
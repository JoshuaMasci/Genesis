cmake_minimum_required(VERSION 3.16.0)
project(Core CXX)

file(GLOB_RECURSE CORE_SOURCES "source/*.*")
file(GLOB_RECURSE CORE_HEADERS "include/*.*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CORE_SOURCES} ${CORE_HEADERS})

add_library(Core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_compile_features(Core INTERFACE cxx_std_17)
target_include_directories(Core PUBLIC include/)
target_include_directories(Core PUBLIC ${CMAKE_SOURCE_DIR}/submodules/concurrentqueue/)
target_include_directories(Core PUBLIC ${CMAKE_SOURCE_DIR}/submodules/glm/)
target_include_directories(Core PUBLIC ${CMAKE_SOURCE_DIR}/submodules/parallel-hashmap/)

set_target_properties(Core PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(Core
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
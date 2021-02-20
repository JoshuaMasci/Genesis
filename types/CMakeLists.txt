cmake_minimum_required(VERSION 3.16.0)
project(Types CXX)

file(GLOB_RECURSE TYPE_SOURCES "source/*.*")
file(GLOB_RECURSE TYPE_HEADERS "include/*.*")

add_library(Types STATIC ${TYPE_SOURCES} ${TYPE_HEADERS})
target_compile_features(Types INTERFACE cxx_std_17)
target_include_directories(Types PUBLIC include/)
target_include_directories(Types PUBLIC ${CMAKE_SOURCE_DIR}/submodules/concurrentqueue/)
target_include_directories(Types PUBLIC ${CMAKE_SOURCE_DIR}/submodules/glm/)
target_include_directories(Types PUBLIC ${CMAKE_SOURCE_DIR}/submodules/parallel-hashmap/)

set_target_properties(Types PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(Types
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
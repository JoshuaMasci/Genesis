cmake_minimum_required(VERSION 3.8.0)
project(Shader_Util CXX)

file(GLOB_RECURSE GENESIS_SOURCES "source/*.*")
file(GLOB_RECURSE GENESIS_HEADERS "include/*.*")

add_executable(Shader_Permutation source/ShaderPermutation.cpp)

target_include_directories(Shader_Permutation PUBLIC ${CMAKE_SOURCE_DIR}/lib/jsoncons-0.148.0/include/)

target_compile_features(Shader_Permutation PUBLIC cxx_std_17)

set_target_properties(Shader_Permutation PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${Shader_Util_SOURCE_DIR}")

set_target_properties(Shader_Permutation
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
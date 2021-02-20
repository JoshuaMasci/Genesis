cmake_minimum_required(VERSION 3.16.0)
project(Sandbox CXX)

file(GLOB_RECURSE SANDBOX_SOURCES "source/*.*")
file(GLOB_RECURSE SANDBOX_HEADERS "include/*.*")

add_executable(Sandbox ${SANDBOX_SOURCES} ${SANDBOX_HEADERS})

target_include_directories(Sandbox PUBLIC include/)

target_compile_features(Sandbox INTERFACE cxx_std_17)

#Working Directory
set_target_properties(Sandbox PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${Sandbox_SOURCE_DIR}")

set_target_properties(Sandbox
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
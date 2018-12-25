cmake_minimum_required(VERSION 3.8.0)
project(Sandbox)

#option (SDL2_ENTRY_POINT "Use the SDL2 entry point" ON) 

include_directories(src)
file(GLOB_RECURSE SANDBOX_SOURCES "src/Sandbox/*.cpp")
file(GLOB_RECURSE SANDBOX_HEADERS "src/Sandbox/*.hpp")
source_group("Sandbox" FILES ${SANDBOX_SOURCES})
source_group("Sandbox_Headers" FILES ${SANDBOX_HEADERS})

include_directories(${Genesis_SOURCE_DIR}/src)

#GLM is the only other lib needed
include_directories(${CMAKE_SOURCE_DIR}/lib/glm/)

add_executable(Sandbox ${SANDBOX_SOURCES} ${SANDBOX_HEADERS})
target_link_libraries(Sandbox Genesis)
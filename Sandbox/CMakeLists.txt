cmake_minimum_required(VERSION 3.8.0)
project(Sandbox)

include_directories(src)
file(GLOB_RECURSE SANDBOX_SOURCES "src/Sandbox/*.cpp")
file(GLOB_RECURSE SANDBOX_HEADERS "src/Sandbox/*.hpp")
source_group("Sandbox" FILES ${SANDBOX_SOURCES})
source_group("Sandbox_Headers" FILES ${SANDBOX_HEADERS})

include_directories(${Genesis_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/lib/glm/)
include_directories(${CMAKE_SOURCE_DIR}/lib/entt-master/src/)

link_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/)
link_directories(${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/lib/Release/x64/)
link_directories(${CMAKE_SOURCE_DIR}/lib/bullet3-2.86.1/build/lib/)

add_executable(Sandbox ${SANDBOX_SOURCES} ${SANDBOX_HEADERS})
set_target_properties(Sandbox PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${Sandbox_SOURCE_DIR}")

target_link_libraries(Sandbox Genesis)
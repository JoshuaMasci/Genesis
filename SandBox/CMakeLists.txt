cmake_minimum_required(VERSION 3.8.0)
project(Sandbox)

include_directories(src)
file(GLOB_RECURSE SANDBOX_SOURCES "src/Sandbox/*.cpp")
file(GLOB_RECURSE SANDBOX_HEADERS "src/Sandbox/*.hpp")
source_group("Sandbox" FILES ${SANDBOX_SOURCES})
source_group("Sandbox_Headers" FILES ${SANDBOX_HEADERS})

include_directories(${Genesis_Engine_SOURCE_DIR}/src)

include_directories(${CMAKE_SOURCE_DIR}/lib/glm/)

include_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.4/include/)
link_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.4/lib/x64/)

link_directories(${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/lib/Release/x64/)
link_directories(${CMAKE_SOURCE_DIR}/lib/reactphysics3d-master/build/lib/Debug/)

add_executable(Sandbox ${SANDBOX_SOURCES} ${SANDBOX_HEADERS})

link_directories(${Genesis_Engine_BINARY_DIR})
target_link_libraries(Sandbox PRIVATE Genesis)
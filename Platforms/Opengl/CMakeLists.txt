cmake_minimum_required(VERSION 3.16.0)
project(OpenGL_Rendering CXX)

file(GLOB_RECURSE OPENGL_SOURCES "source/*.*")
file(GLOB_RECURSE OPENGL_HEADERS "include/*.*")

add_library(OpenGL_Rendering STATIC ${OPENGL_SOURCES} ${OPENGL_HEADERS})
target_compile_features(OpenGL_Rendering PRIVATE cxx_std_17)
target_include_directories(OpenGL_Rendering PUBLIC include/)

target_link_libraries(OpenGL_Rendering PUBLIC Genesis_Engine)
target_link_libraries(OpenGL_Rendering PUBLIC SDL2_Platform)

target_link_libraries (OpenGL_Rendering PUBLIC opengl32)
target_link_libraries (OpenGL_Rendering PUBLIC glu32)
target_include_directories(OpenGL_Rendering PUBLIC ${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/include/)
target_link_libraries (OpenGL_Rendering PUBLIC ${CMAKE_SOURCE_DIR}/lib/glew-1.12.0/lib/Release/x64/glew32.lib)

set_target_properties(OpenGL_Rendering
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)


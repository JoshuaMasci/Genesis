cmake_minimum_required(VERSION 3.16.0)
project(Genesis_Editor CXX)

file(GLOB_RECURSE GENESIS_EDITOR_SOURCES "source/*.*")
file(GLOB_RECURSE GENESIS_EDITOR_HEADERS "include/*.*")

add_executable(Genesis_Editor ${GENESIS_EDITOR_SOURCES} ${GENESIS_EDITOR_HEADERS})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GENESIS_EDITOR_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GENESIS_EDITOR_HEADERS})

target_include_directories(Genesis_Editor PUBLIC include/)

target_compile_features(Genesis_Editor INTERFACE cxx_std_17)

#Working Directory
set_target_properties(Genesis_Editor PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${Genesis_Editor_SOURCE_DIR}")

target_link_libraries(Genesis_Editor PUBLIC Genesis_Engine)

if(INCLUDE_EASY_PROFILER)
	add_custom_command(TARGET Genesis_Editor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/lib/easy_profiler-v2.1.0-msvc15-win64/bin/easy_profiler.dll" $<TARGET_FILE_DIR:Genesis_Editor>)
endif()

#Plaftorms
#SDL2
target_link_libraries(Genesis_Editor PUBLIC SDL2_Platform)
add_custom_command(TARGET Genesis_Editor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/SDL2.dll" $<TARGET_FILE_DIR:Genesis_Editor>)

#VulkanBackend
#target_link_libraries(Genesis_Editor PUBLIC Vulkan_Rendering)

#OpenglBackend
target_link_libraries(Genesis_Editor PUBLIC OpenGL_Rendering)

set_target_properties(Genesis_Editor
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
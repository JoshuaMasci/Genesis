cmake_minimum_required(VERSION 3.8.0)
project(Sandbox CXX)

file(GLOB_RECURSE SANDBOX_SOURCES "source/*.*")
file(GLOB_RECURSE SANDBOX_HEADERS "include/*.*")

add_executable(Sandbox ${SANDBOX_SOURCES} ${SANDBOX_HEADERS})

target_include_directories(Sandbox PUBLIC include/)

target_compile_features(Sandbox INTERFACE cxx_std_17)
#target_compile_features(Sandbox PUBLIC cxx_auto_type cxx_constexpr)

#Working Directory
set_target_properties(Sandbox PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${Sandbox_SOURCE_DIR}")

target_link_libraries(Sandbox PUBLIC Genesis_Engine)

if(INCLUDE_EASY_PROFILER)
	add_custom_command(TARGET Sandbox POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/lib/easy_profiler-v2.1.0-msvc15-win64/bin/easy_profiler.dll" $<TARGET_FILE_DIR:Sandbox>)
endif()

#Plaftorms
#SDL2
target_link_libraries(Sandbox PUBLIC SDL2_Platform)
add_custom_command(TARGET Sandbox POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/SDL2.dll" $<TARGET_FILE_DIR:Sandbox>)

#VulkanBackend
target_link_libraries(Sandbox PUBLIC Vulkan_Rendering)
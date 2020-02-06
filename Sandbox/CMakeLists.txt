cmake_minimum_required(VERSION 3.8.0)
project(Sandbox)

include_directories(src)

file(GLOB_RECURSE SANDBOX_SOURCES "src/Sandbox/*.*")
source_group("Sandbox" FILES ${SANDBOX_SOURCES})

include_directories(${Genesis_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/lib/glm/)
include_directories(${CMAKE_SOURCE_DIR}/lib/concurrent_queue/)
include_directories(${CMAKE_SOURCE_DIR}/lib/stb/)
include_directories(${CMAKE_SOURCE_DIR}/lib/tinyobj/)
include_directories(${CMAKE_SOURCE_DIR}/lib/imgui/)
include_directories(${CMAKE_SOURCE_DIR}/lib/spdlog-1.5.0/include)

link_directories(${CMAKE_SOURCE_DIR}/lib/assimp-3.3.1/build/code/Debug/)
link_directories(${CMAKE_SOURCE_DIR}/lib/assimp-3.3.1/build/contrib/zlib/Debug/)

include_directories(${CMAKE_SOURCE_DIR}/lib/reactphysics3d-master/src/)
link_directories(${CMAKE_SOURCE_DIR}/lib/reactphysics3d-master/build/lib/)
add_definitions(-DIS_DOUBLE_PRECISION_ENABLED)

#TEMP ENTT
include_directories(${CMAKE_SOURCE_DIR}/lib/entt-3.2.2/src/)

if(INCLUDE_EASY_PROFILER)
	include_directories(${CMAKE_SOURCE_DIR}/lib/easy_profiler-v2.1.0-msvc15-win64/include/)
	link_directories(${CMAKE_SOURCE_DIR}/lib/easy_profiler-v2.1.0-msvc15-win64/lib)
	add_definitions(-DGENESIS_PROFILER_ENABLED)
endif()

add_executable(Sandbox ${SANDBOX_SOURCES} ${SANDBOX_HEADERS})
set_target_properties(Sandbox PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${Sandbox_SOURCE_DIR}")

if(INCLUDE_SDL2)
	addSDL2(Sandbox)
endif()

if(INCLUDE_VULKAN)
	addVulkan(Sandbox)
endif()

target_link_libraries(Sandbox Genesis)

#Copy DLLs to runtime directories
#add_custom_command(TARGET Sandbox POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/SDL2.dll" $<TARGET_FILE_DIR:Sandbox>)
add_custom_command(TARGET Sandbox POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/lib/easy_profiler-v2.1.0-msvc15-win64/bin/easy_profiler.dll" $<TARGET_FILE_DIR:Sandbox>)

#TEMP
add_custom_command(TARGET Sandbox POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/lib/assimp-3.3.1/build/code/Debug/assimp-vc140-mt.dll" $<TARGET_FILE_DIR:Sandbox>)
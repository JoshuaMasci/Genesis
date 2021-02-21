cmake_minimum_required(VERSION 3.16.0)
project(Genesis_Engine CXX)

file(GLOB_RECURSE GENESIS_SOURCES "source/*.*")
file(GLOB_RECURSE GENESIS_HEADERS "include/*.*")

file (GLOB IMGUI_SOURCES ${CMAKE_SOURCE_DIR}/submodules/imgui/*.*cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GENESIS_SOURCES} ${GENESIS_HEADERS})
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${IMGUI_SOURCES})
add_library(Genesis_Engine STATIC ${GENESIS_SOURCES} ${GENESIS_HEADERS} ${IMGUI_SOURCES})

target_include_directories(Genesis_Engine PUBLIC include/)
target_precompile_headers(Genesis_Engine PUBLIC "include/genesis_engine/pch.hpp")
target_compile_definitions(Genesis_Engine PUBLIC GENESIS_PLATFORM_WIN)
target_compile_features(Genesis_Engine PUBLIC cxx_std_17)

target_link_libraries(Genesis_Engine PUBLIC Core)
target_link_libraries(Genesis_Engine PUBLIC VulkanRenderer)

target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/submodules/yaml-cpp/include/)
target_link_libraries(Genesis_Engine PUBLIC debug ${CMAKE_SOURCE_DIR}/submodules/yaml-cpp/build/Debug/yaml-cppd.lib)
target_link_libraries(Genesis_Engine PUBLIC optimized ${CMAKE_SOURCE_DIR}/submodules/yaml-cpp/build/Release/yaml-cpp.lib)
 
target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/submodules/concurrentqueue/)
target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/submodules/entt/src/)
target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/submodules/glm/)
target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/submodules/imgui/)
target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/submodules/ImGuizmo/)
target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/submodules/parallel-hashmap/)
target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/submodules/spdlog/include/)
target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/submodules/stb/)
target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/submodules/tinyobjloader/)

if(INCLUDE_EASY_PROFILER)
	target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/lib/easy_profiler-v2.1.0-msvc15-win64/include/)
	target_link_libraries(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/lib/easy_profiler-v2.1.0-msvc15-win64/lib/easy_profiler.lib)
	target_compile_definitions(Genesis_Engine PUBLIC GENESIS_PROFILER_ENABLED)
endif()

set_target_properties(Genesis_Engine
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
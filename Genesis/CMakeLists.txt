cmake_minimum_required(VERSION 3.8.0)
project(Genesis_Engine CXX)

file(GLOB_RECURSE GENESIS_SOURCES "source/*.*")
file(GLOB_RECURSE GENESIS_HEADERS "include/*.*")

add_library(Genesis_Engine STATIC ${GENESIS_SOURCES} ${GENESIS_HEADERS})

target_include_directories(Genesis_Engine PUBLIC include/)

target_compile_definitions(Genesis_Engine PUBLIC GENESIS_PLATFORM_WIN)

target_compile_features(Genesis_Engine INTERFACE cxx_std_17)
#target_compile_features(Genesis_Engine PUBLIC cxx_auto_type cxx_constexpr)

target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/lib/glm/)
target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/lib/concurrent_queue/)
target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/lib/array-hash-0.7.0/include/)
target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/lib/spdlog-1.5.0/include/)
target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/lib/imgui/)
target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/lib/tinyobj/)
target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/lib/stb/)

target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/lib/reactphysics3d-master/src/)
target_link_libraries(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/lib/reactphysics3d-master/build/lib/debug/reactphysics3d.lib)
target_compile_definitions(Genesis_Engine PUBLIC IS_DOUBLE_PRECISION_ENABLED)

target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/lib/)

if(INCLUDE_EASY_PROFILER)
	target_include_directories(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/lib/easy_profiler-v2.1.0-msvc15-win64/include/)
	target_link_libraries(Genesis_Engine PUBLIC ${CMAKE_SOURCE_DIR}/lib/easy_profiler-v2.1.0-msvc15-win64/lib/easy_profiler.lib)
	target_compile_definitions(Genesis_Engine PUBLIC GENESIS_PROFILER_ENABLED)
endif()
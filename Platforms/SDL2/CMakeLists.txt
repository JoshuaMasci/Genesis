cmake_minimum_required(VERSION 3.8.0)
project(SDL2_Platform CXX)

file(GLOB_RECURSE SDL2_SOURCES "source/*.*")
file(GLOB_RECURSE SDL2_HEADERS "include/*.*")

add_library(SDL2_Platform STATIC ${SDL2_SOURCES} ${SDL2_HEADERS})

target_include_directories(SDL2_Platform PUBLIC include/)

target_compile_features(SDL2_Platform INTERFACE cxx_std_17)
#target_compile_features(SDL2_Platform PUBLIC cxx_auto_type cxx_constexpr)

target_link_libraries(SDL2_Platform PUBLIC Genesis_Engine)

target_include_directories(SDL2_Platform PUBLIC ${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/include/)
target_link_libraries(SDL2_Platform PUBLIC ${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/SDL2.lib)
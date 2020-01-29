cmake_minimum_required(VERSION 3.8.0)
project(SDL2_Platform)

include_directories(${Genesis_SOURCE_DIR})

file(GLOB SDL2_SOURCES "*")
source_group("source" FILES ${SDL2_SOURCES})

#Create Libary
add_library(SDL2_Platform STATIC ${SDL2_SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/include/)
target_link_libraries(SDL2_Platform ${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/SDL2.lib)


set_target_properties(SDL2_Platform
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

macro(addSDL2 TARGET)
	target_link_libraries(${TARGET} SDL2_Platform)
	target_link_libraries(${TARGET} ${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/SDL2.lib)
	include_directories(${TARGET} ${CMAKE_SOURCE_DIR}/Platforms/SDL2/)
	add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/lib/x64/SDL2.dll" $<TARGET_FILE_DIR:${TARGET}>)
endmacro()
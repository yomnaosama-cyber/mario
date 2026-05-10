cmake_minimum_required(VERSION 3.16)
project(MarioScene)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include_directories(${CMAKE_SOURCE_DIR}/include)

# Automatically find all source files
file(GLOB SOURCES 
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

file(GLOB HEADERS 
    "${CMAKE_SOURCE_DIR}/include/*.h"
)

add_executable(mario_scene ${SOURCES} ${HEADERS})

target_link_libraries(mario_scene Qt6::Core Qt6::Widgets)

set_target_properties(mario_scene PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_custom_command(TARGET mario_scene POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "${CMAKE_BINARY_DIR}/assets"
    COMMENT "Copying assets to build directory..."
)

message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}/src")
message(STATUS "Include directory: ${CMAKE_SOURCE_DIR}/include")
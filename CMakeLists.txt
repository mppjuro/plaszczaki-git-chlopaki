cmake_minimum_required(VERSION 3.10)
project(plaszczaki)

# Set the directory where the sources are
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

add_definitions(-DSDL_MAIN_HANDLED)

# Include directories
set(SDL2_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/include")
include_directories(${SDL2_INCLUDE_DIR})

# Set the output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Source files
set(SOURCE_FILES
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/functions.cpp
)

# Add executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)
add_executable(plaszczaki WIN32 ${SOURCE_FILES})

# Directly set the paths to the SDL libraries
set(SDL2_DLL "${CMAKE_SOURCE_DIR}/lib/bin/SDL2.dll")
set(SDL2_TTF_DLL "${CMAKE_SOURCE_DIR}/lib/bin/SDL2_ttf.dll")

# Use -l: option to link directly against the .dll for MinGW
target_link_libraries(plaszczaki
    "-L${CMAKE_SOURCE_DIR}/lib/bin" -l:SDL2.dll -l:SDL2_ttf.dll
)

# Copy SDL2 DLLs to the output directory
add_custom_command(TARGET plaszczaki POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_DLL}"
        "${SDL2_TTF_DLL}"
        $<TARGET_FILE_DIR:plaszczaki>)

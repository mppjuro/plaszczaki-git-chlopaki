cmake_minimum_required(VERSION 3.10)
project(plaszczaki)

set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -lSDL2 -lSDL2_ttf")

# Lista katalogów do stworzenia
set(DIRS
    ../data/input
    ../data/junk
    ../data/results
)

# Tworzenie katalogów
foreach(dir ${DIRS})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${dir})
endforeach()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)
add_executable(plaszczaki ${SOURCE_DIR}/main.cpp ${SOURCE_DIR}/functions.cpp)

cmake_minimum_required(VERSION 3.10)
project(plaszczaki)

set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

# Lista katalogów do stworzenia
set(DIRS
    ../data/prob1
    ../data/prob2
)

# Tworzenie katalogów
foreach(dir ${DIRS})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${dir})
endforeach()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)
add_executable(plaszczaki ${SOURCE_DIR}/main.cpp ${SOURCE_DIR}/helpers.cpp ${SOURCE_DIR}/huffman.cpp)

target_link_libraries(${PROJECT_NAME} SDL2 SDL2_ttf)

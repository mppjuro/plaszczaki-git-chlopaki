cmake_minimum_required(VERSION 3.10)
project(plaszczaki)

set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

set(SDL2_INCLUDE_DIR "D:/Dev/Libs/SDL2/x86_64-w64-mingw32/include")
set(SDL2_LIBRARIES "D:/Dev/Libs/SDL2/x86_64-w64-mingw32/lib")

include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIBRARIES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

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

target_link_libraries(${PROJECT_NAME})
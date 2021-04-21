cmake_minimum_required(VERSION 3.5)
project(chip8-cpp)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(chip8
    src/main.cpp
    src/Chip8.cpp
    src/Mem.cpp
    src/Display.cpp
)
target_link_libraries(chip8 ${SDL2_LIBRARIES})

cmake_minimum_required(VERSION 3.16)

project(chip8)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)

add_executable(
    chip8
    main.cpp
    chip8.cpp
    platform.cpp
)

target_link_libraries(chip8 PRIVATE
    PkgConfig::SDL2
)

target_compile_options(chip8 PRIVATE
    -Wall
    -Wextra
)
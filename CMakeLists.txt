cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(/W4)

project(mcugif)

add_subdirectory(src)
add_subdirectory(spec)

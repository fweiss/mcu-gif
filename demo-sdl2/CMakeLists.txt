cmake_minimum_required(VERSION 3.17)

project(demo)
add_subdirectory(../src ../build/main)

add_executable(demo main.c reader.c)
target_include_directories(demo PRIVATE "/Library/Frameworks/SDL2.framework/Headers")
target_include_directories(demo PRIVATE "../src")
target_link_libraries(demo main)
target_link_libraries(demo "/Library/Frameworks/SDL2.framework/SDL2")

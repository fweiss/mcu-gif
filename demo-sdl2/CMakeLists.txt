cmake_minimum_required(VERSION 3.17)
project(demo-sdl2)
set(CMAKE_C_STANDARD 99)
file(GLOB SRC main.c reader.c)

set(SDL2_INCLUDE "e:/Libraries/SDL2-2.32.2/include")
set(SDL2_LIB "e:/Libraries/SDL2-2.32.2/lib/x86/SDL2.lib")


add_executable(demo-sdl2 ${SRC})
include_directories(${SDL2_INCLUDE} ../src)
# link_libraries("e:/Libraries/SDL2.dll" SHARED IMPORTED)
target_link_libraries(demo-sdl2 ${SDL2_LIB} ../../build/src/main)
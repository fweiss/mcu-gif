cmake_minimum_required(VERSION 3.17)

project(demo-sdl2)
set(CMAKE_C_STANDARD 99)

# todo get this from environment variable
set(SDL2 "e:/Libraries/SDL2-2.32.2")
set(SDL2_INC ${SDL2}/include)
set(SDL2_LIB ${SDL2}/lib/x86/SDL2.lib)
set(SDL2_DLL ${SDL2}/lib/x86/SDL2.dll)

file(GLOB SRC main.c reader.c)

add_executable(demo-sdl2 ${SRC})
include_directories(${SDL2_INC} ../src)
target_link_libraries(demo-sdl2 ${SDL2_LIB} ../../build/src/main)

# this could be done with PATH, but copying is more explicit
file(COPY ${SDL2_DLL} DESTINATION ${CMAKE_BINARY_DIR})

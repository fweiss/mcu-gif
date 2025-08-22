cmake_minimum_required(VERSION 3.17)

project(demo-sdl2)
set(CMAKE_C_STANDARD 99)

# todo get this from environment variable
if (MSVC)
    set(SDL2 "e:/Libraries/SDL2-2.32.2")
    set(SDL2_INC ${SDL2}/include)
    set(SDL2_LIB ${SDL2}/lib/x86/SDL2.lib)
    set(SDL2_DLL ${SDL2}/lib/x86/SDL2.dll)
else()
    set(SDL2 "/Library/Frameworks/SDL2.framework")
    set(SDL2_INC ${SDL2}/Headers)
    set(SDL2_LIB ${SDL2}/SDL2)
endif()

file(GLOB SRC main.c reader.c)

add_executable(demo-sdl2 ${SRC})
include_directories(${SDL2_INC} ../src)
# fixme this should be a linked cmake project
if(MSVC)
target_link_libraries(demo-sdl2 ${SDL2_LIB} ../../build/src/main)
else()
target_link_libraries(demo-sdl2 ${SDL2_LIB} ${CMAKE_SOURCE_DIR}/../build/src/libmain.a)
endif()

if(MSVC)
# this could be done with PATH, but copying is more explicit
file(COPY ${SDL2_DLL} DESTINATION ${CMAKE_BINARY_DIR})
endif()

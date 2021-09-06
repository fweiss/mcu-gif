cmake_minimum_required(VERSION 3.17)

project(demo-sdl2)

add_subdirectory("../src" "gd")

add_executable(demo-sdl2 main.c reader.c)

target_include_directories(demo-sdl2 PRIVATE /Library/Frameworks/SDL2.framework/Headers)
target_include_directories(demo-sdl2 PRIVATE ../src)

add_library(sdl2 STATIC IMPORTED)
set_target_properties(sdl2 PROPERTIES IMPORTED_LOCATION /Library/Frameworks/SDL2.framework/SDL2)
target_link_libraries(demo-sdl2 PRIVATE sdl2)

add_library(gd SHARED IMPORTED)
# set_target_properties(gd PROPERTIES IMPORTED_LOCATION ../build/src)
target_link_libraries(demo-sdl2 PUBLIC gd)

install(TARGETS demo-sdl2)

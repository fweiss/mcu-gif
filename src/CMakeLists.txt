cmake_minimum_required(VERSION 3.17)

project(main C)

if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
endif()

# restrict C to lowest common standard
set(CMAKE_C_STANDARD 99)

set_source_files_properties(gd.c LANGUAGE C)

file(GLOB_RECURSE SRC "*.h" "*.c")
add_library(main SHARED ${SRC})
#set_target_properties(main PROPERTIES LINKER_LANGUAGE CXX)

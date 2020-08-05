cmake_minimum_required(VERSION 3.17)

if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
endif()

#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

project(main)
file(GLOB_RECURSE SRC "*.h" "*.c")
add_library(main SHARED ${SRC})
#set_target_properties(main PROPERTIES LINKER_LANGUAGE CXX)

cmake_minimum_required(VERSION 3.17)

project(spec)

if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory("../submodules/ccspec/src" "build")

# but ultimately lib
file(GLOB MAIN "../src/*.c")

file(GLOB_RECURSE SOURCES "*.cc")
add_executable(start ${SOURCES})

target_include_directories(start PUBLIC "../submodules/ccspec/include")
target_include_directories(start PUBLIC "../src")

# for mac
#set(CMAKE_MACOSX_RPATH OFF)

#set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX})
#find_library(
#	LIBCCSPEC
#	libccspec.0.1.0.dylib
#	PATHS
#		/Users/frankw/Projects/TDD/cmake-mcu-gif/submodules/ccspec/build/src)

# file(GLOB_RECURSE MAIN "../src/*.h" "../src/*.c")
# add_library(main2 ${MAIN})

target_link_libraries(start PUBLIC ccspec)
target_link_libraries(start PUBLIC main)



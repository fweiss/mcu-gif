cmake_minimum_required(VERSION 3.17)

if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(mcu-gif)
#add_subdirectory("../submodules/ccspec" "build")

# but ultimately lib
file(GLOB MAIN "../src/*.c")

file(GLOB_RECURSE SOURCES "*.cc")
add_executable(start ${SOURCES} "/Users/frankw/Projects/TDD/cmake-mcu-gif/src/gd.c")

target_include_directories(start PUBLIC "/Users/frankw/Projects/TDD/cmake-mcu-gif/submodules/ccspec/include")
target_include_directories(start PUBLIC "../src")

# for mac
#set(CMAKE_MACOSX_RPATH OFF)

#add_library(ccspec SHARED)

set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX})
find_library(
	LIBCCSPEC
	libccspec.0.1.0.dylib
	PATHS
		/Users/frankw/Projects/TDD/cmake-mcu-gif/submodules/ccspec/build/src)

target_link_libraries(start /Users/frankw/Projects/TDD/cmake-mcu-gif/src/build/libmain.dylib)
target_link_libraries(start /Users/frankw/Projects/TDD/cmake-mcu-gif/submodules/ccspec/build/src/libccspec.0.1.0.dylib)
#target_link_libraries(start ccspec)



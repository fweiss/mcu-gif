cmake_minimum_required(VERSION 3.17)

project(spec)

# magic to link with ccspec.lib
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "./bin" CACHE PATH "..." FORCE)
endif()

file(GLOB_RECURSE SOURCES "*.cc")
add_executable(start ${SOURCES})

# for mac
#set(CMAKE_MACOSX_RPATH OFF)

#set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX})
#find_library(
#	LIBCCSPEC
#	libccspec.0.1.0.dylib
#	PATHS
#		/Users/frankw/Projects/TDD/cmake-mcu-gif/submodules/ccspec/build/src)

# just the runtime
add_subdirectory("../submodules/ccspec/src" "ccspec")
target_link_libraries(start PRIVATE ccspec)

target_link_libraries(start PUBLIC main)

install(TARGETS start ccspec RUNTIME DESTINATION .)

cmake_minimum_required(VERSION 3.17)

project(spec)

# magic to link with ccspec.lib
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "./bin" CACHE PATH "..." FORCE)
endif()

file(GLOB_RECURSE SOURCES "*.cc")
add_executable(start ${SOURCES})

target_include_directories(start PUBLIC ../src)
target_include_directories(start PUBLIC ../submodules/ccspec/include)

# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# just the runtime
add_subdirectory("../submodules/ccspec/src" "ccspec")
target_link_libraries(start PRIVATE ccspec)

target_link_libraries(start PUBLIC main)

install(TARGETS start ccspec RUNTIME DESTINATION .)

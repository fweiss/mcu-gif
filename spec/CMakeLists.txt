cmake_minimum_required(VERSION 3.17)

# problems
# run spec\start, then ccspec.dll is missing
# but it is in spec\ccspec, as well as ccspec.lib
# tried to do static, but then get linked errors

project(spec)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "./bin" CACHE PATH "..." FORCE)
endif()

file(GLOB_RECURSE SOURCES "*.cc")
add_executable(start ${SOURCES})
# set(CMAKE_EXE_LINKER_FLAGS "-lz")

# set_target_properties(start PROPERTIES COMPILE_FLAGS -DBUILDER_STATIC_DEFINE)
# set_target_properties(start PROPERTIES COMPILE_DEFINITIONS BUILDER_STATIC_DEFINE)
# set_target_properties(start PROPERTIES IMPORTED_LOCATION )

# set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
# target_compile_options(start PUBLIC /MT)
# target_link_options(start PUBLIC /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)

# set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
# target_compile_options(start PUBLIC /MT)
# target_link_options(start PUBLIC /INCREMENTAL:NO /NODEFAULTLIB:MSVCRTD)
# target_link_options(start PUBLIC /WHOLEARCHIVE:ccspec/ccspec.lib)

target_include_directories(start PUBLIC ../src)
target_include_directories(start PUBLIC ../submodules/ccspec/include)

# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# just the runtime
add_subdirectory("../submodules/ccspec/src" "ccspec")
target_link_libraries(start PUBLIC ccspec)
# target_link_libraries(start PRIVATE ccspec)

target_link_libraries(start PUBLIC main)

install(TARGETS start ccspec RUNTIME DESTINATION .)

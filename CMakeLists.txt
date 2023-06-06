project(FMFIG-code)
cmake_minimum_required(VERSION 3.1)
set(CMAKE_SKIP_INSTALL_RULES True)

# Compiler requirements
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Activate all warnings
if(CMAKE_COMPILER_IS_GNUCC)
    add_definitions ("-Wall")
    add_definitions ("-std=c++11")
endif()
if ( MSVC )
    add_definitions ("/W4")
endif()


include_directories(.)
include_directories(simplem)

#Simple examples and tests
add_executable(example1 examples/example1.cpp fbx.cpp fbx.hpp miniz.c)
add_executable(example2 examples/example2.cpp fbx.cpp fbx.hpp miniz.c)
add_executable(example3 examples/example3.cpp fbx.cpp fbx.hpp miniz.c)

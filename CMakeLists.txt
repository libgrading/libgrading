cmake_minimum_required(VERSION 2.8.12)
project (libgrading)

add_definitions("-std=c++11")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_INSTALL_NAME_DIR @rpath)
set(CMAKE_BUILD_WITH_INSTALL_RPATH on)

include(CheckIncludeFiles)
check_include_files("sys/mman.h;unistd.h" POSIX)

include_directories(include)

add_subdirectory(doc)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)

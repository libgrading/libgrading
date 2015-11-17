cmake_minimum_required(VERSION 2.8.12)
project (libgrading)

#
# We use libdistance for fuzzy string matching.
#
find_path(LIBDISTANCE_INCLUDE_DIR distance.h)
find_library(LIBDISTANCE distance)

if (LIBDISTANCE_INCLUDE_DIR)
	message("-- Found distance.h in ${LIBDISTANCE_INCLUDE_DIR}")
	include_directories(${LIBDISTANCE_INCLUDE_DIR})
else ()
	message(FATAL_ERROR
		"Unable to find libdistance header 'distance.h'"
		" in standard include paths.")
endif ()

if (LIBDISTANCE)
	message("-- Found libdistance: ${LIBDISTANCE}")
	get_filename_component(LIBDISTANCE_DIR ${LIBDISTANCE} DIRECTORY CACHE)
	link_directories(${LIBDISTANCE_DIR})
else()
	message(FATAL_ERROR
		"Unable to find libdistance in standard library paths")
endif ()


add_definitions("-std=c++11")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

set(CMAKE_INSTALL_NAME_DIR @rpath)
set(CMAKE_BUILD_WITH_INSTALL_RPATH on)

include(CheckIncludeFiles)
check_include_files("sys/mman.h;unistd.h" POSIX)

include_directories(include)

add_subdirectory(doc)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)

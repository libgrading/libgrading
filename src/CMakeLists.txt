if (POSIX)
	set(PLATFORM_SOURCES "posix.cpp")
else()
	message(FATAL_ERROR
		"libgrading currently works on POSIX platforms only.\n"
		"Code contributions for Windows would be welcome.")
endif ()


#
# We use libdistance for fuzzy string matching.
#
find_path(LIBDISTANCE_INCLUDE_DIR distance.h)
find_library(LIBDISTANCE distance)

if (LIBDISTANCE_INCLUDE_DIR)
	message("-- Found distance.h in ${LIBDISTANCE_INCLUDE_DIR}")
else ()
	message(FATAL_ERROR
		"Unable to find libdistance header 'distance.h'"
		" in standard include paths.")
endif ()

if (LIBDISTANCE)
	message("-- Found libdistance: ${LIBDISTANCE}")
else()
	message(FATAL_ERROR
		"Unable to find libdistance in standard library paths")
endif ()


add_library(grading SHARED
	checks.cpp
	TestResult.cpp
	${PLATFORM_SOURCES}
)

target_link_libraries(grading ${LIBDISTANCE})

install(TARGETS grading LIBRARY DESTINATION lib)

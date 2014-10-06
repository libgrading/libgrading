if (POSIX)
	set(PLATFORM_SOURCES "posix.cpp")
else()
	message(FATAL_ERROR
		"libgrading currently works on POSIX platforms only.\n"
		"Code contributions for Windows would be welcome.")
endif ()

add_library(grading SHARED
	checks.cpp
	TestResult.cpp
	${PLATFORM_SOURCES}
)

install(TARGETS grading LIBRARY DESTINATION lib)

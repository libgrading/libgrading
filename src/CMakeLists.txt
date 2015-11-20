if (POSIX)
	set(PLATFORM_SOURCES "posix.cpp")
else()
	message(FATAL_ERROR
		"libgrading currently works on POSIX platforms only.\n"
		"Code contributions for Windows would be welcome.")
endif ()


add_library(grading SHARED
	Arguments.cpp
	checks.cpp
	strategy.cpp
	TestResult.cpp
	${PLATFORM_SOURCES}
)

target_link_libraries(grading ${LIBDISTANCE})

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	target_link_libraries(grading rt)
endif ()

install(TARGETS grading LIBRARY DESTINATION lib)

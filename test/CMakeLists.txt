add_executable(test-libgrading test.cpp)

set_target_properties(test-libgrading PROPERTIES
	COMPILE_FLAGS "-Wno-missing-prototypes -Wno-global-constructors -Wno-exit-time-destructors")

target_link_libraries(test-libgrading grading)

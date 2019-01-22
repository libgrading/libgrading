foreach (testname skip test)
	set(test_binary "test-${testname}")

	add_executable(${test_binary} "${testname}.cpp")

	set_target_properties(${test_binary} PROPERTIES
		COMPILE_FLAGS
			"-Wno-missing-prototypes -Wno-global-constructors -Wno-exit-time-destructors"
	)

	target_link_libraries(${test_binary} grading)
endforeach (testname)

add_test(NAME skip COMMAND test-skip --skip)
add_test(NAME tests COMMAND test-test)

set_tests_properties(tests PROPERTIES
	ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src")

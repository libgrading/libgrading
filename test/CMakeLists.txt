set (LIBPATH "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src")
set (TEST_CFLAGS "${TEST_CFLAGS} -Wno-exit-time-destructors")
set (TEST_CFLAGS "${TEST_CFLAGS} -Wno-global-constructors")
set (TEST_CFLAGS "${TEST_CFLAGS} -Wno-missing-prototypes")

function (add_libgrading_test name)
    set(binary "test-${name}")

    add_executable(${binary} "${name}.cpp")
    set_target_properties(${binary} PROPERTIES COMPILE_FLAGS ${TEST_CFLAGS})
    target_link_libraries(${binary} grading)

    add_test(NAME ${name} COMMAND ${binary} ${ARGN})
    set_tests_properties(${name} PROPERTIES ENVIRONMENT ${LIBPATH})
endfunction (add_libgrading_test)

add_libgrading_test(skip --skip)
add_libgrading_test(gradescope --format=gradescope)
add_libgrading_test(test)

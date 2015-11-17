/*!
 * @file      libgrading.h
 * @brief     A library for grading C- and C++-based assignments.
 *
 * @author    Jonathan Anderson <jonathan.anderson@mun.ca>
 * @copyright (c) 2014-2015 Jonathan Anderson. All rights reserved.
 * @license   Apache License, Version 2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#ifndef LIBGRADING_H
#define LIBGRADING_H

#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>


//! Container for all libgrading names.
namespace grading {


//! Ways that we can run tests.
enum class TestRunStrategy
{
	Inline,      //!< In the same process, in the current call stack.
	Separated,   //!< In separate but unsandboxed processes.
	Sandboxed,   //!< In a separate, sandboxed process (if supported).
};

//! The @ref TestRunStrategy currently in use.
TestRunStrategy CurrentStrategy();

/**
 * Set a new @ref TestRunStrategy.
 *
 * @returns   the previous @ref TestRunStrategy
 */
TestRunStrategy SetStrategy(TestRunStrategy);


class CheckResult
{
	public:
	CheckResult();
	CheckResult(std::string message);
	CheckResult(std::string exected, std::string actual);

	CheckResult(const CheckResult&) = delete;
	CheckResult(CheckResult&&);

	~CheckResult();

	CheckResult& operator << (const std::vector<std::string>&);

	template<class T>
	CheckResult& operator << (const T& x)
	{
		message_ << x;
		return *this;
	}

	bool error() const { return reportError_; }
	void cancel() { reportError_ = false; }

	std::string expected() const { return expected_; }
	std::string actual() const { return actual_; }
	std::string message() const { return message_.str(); }

	private:
	bool reportError_;

	const std::string expected_;
	const std::string actual_;

	std::ostringstream message_;
};

//! Combine the results of two checks using a product (AND): both must pass.
CheckResult operator && (CheckResult&&, CheckResult&&);

//! L-value version of above.
CheckResult operator && (CheckResult&&, CheckResult&);

//! Combine the results of two checks using a sum (OR): at least one must pass.
CheckResult operator || (CheckResult&&, CheckResult&&);

//! L-value version of above.
CheckResult operator || (CheckResult&&, CheckResult&);


//
// Checks for tests:
//

//! Check an arbitrary condition, failing the test if false.
CheckResult Check(bool, std::string description);

//! Check that two integers are equal, failing the test if they are not.
CheckResult CheckInt(int expected, int actual);

//! Check that two floating-point numbers are equal within some tolerance.
CheckResult CheckFloat(double exp, double act, double tolerance = 0.000001);

//! Check that a pointer is not equal to nullptr.
CheckResult CheckNonNull(const void*, std::string message);

//! Check that a pointer is equal to nullptr.
CheckResult CheckNull(const void*, std::string message);

/**
 * Check that two strings are (approximately) equal.
 *
 * @param   expected            the string we expected
 * @param   actual              the string we got
 * @param   maxEditDistance     how fuzzy the match can be: the maximum
 *                              Levenshtein distance between them
 */
CheckResult CheckString(std::string expected, std::string actual,
                        size_t maxEditDistance = 0);


//! The result of running one test within a separate process.
enum class TestResult : char
{
	Pass,                //!< the test succeeded
	Fail,                //!< the test failed
	Abort,               //!< the test was aborted (e.g., assert() fired)
	Segfault,            //!< the test caused a segmentation fault
	Timeout,             //!< the test took too long to run
	UncaughtException,   //!< the test threw an exception
	OtherError           //!< the test terminated for another reason
};

//! Output a human-readable representation of a @ref TestResult.
std::ostream& operator << (std::ostream&, TestResult);


//! A closure that wraps a single test case.
typedef std::function<TestResult ()> TestClosure;


/**
 * A representation of a shared memory object.
 *
 * This class is specialized by platform-specific code to represent
 * shared memory in a way that will be cleaned up on destruction
 * (files closed, memory unmapped, etc.).
 */
class SharedMemory
{
	public:
	virtual ~SharedMemory() {}

	/**
	 * A pointer to the shared memory, which will be @b invalidated
	 * after this object is destructed.
	 */
	virtual void* rawPointer() const = 0;
};


//! Map data into the address space that can be shared with other processes.
std::unique_ptr<SharedMemory> MapSharedData(size_t size);


/**
 * Run a test closure in a separate process, capturing segmentation faults
 * and other errors that lead to termination.
 *
 * @param    test         the test to run
 * @param    name         a developer-friendly name
 * @param    timeout      the number of seconds to let the test run
 *                        (default 0, meaning "no timeout, run forever")
 * @param    errorStream  where to write messages (e.g., "expected X, got Y")
 *
 * @returns  the result of the test, either pass/fail as reported by @b test
 *           or else segfault/other as captured by libgrading
 */
TestResult RunTest(TestClosure test, std::string name = "<unnamed test>",
                   time_t timeout = 0, std::ostream& errorStream = std::cerr);


/**
 * Run a test with output but no explicit input.
 *
 * @param    test    a test function that copies out an output value
 *                   and returns a @ref TestResult
 * @param    output  the output value produced by @b test
 *                   (if it executes normally, e.g., without segfaulting)
 * @param    name    a developer-friendly name
 * @param    timeout      the number of seconds to let the test run
 *                        (default 0, meaning "no timeout, run forever")
 * @param    errorStream  where to write messages (e.g., "expected X, got Y")
 *
 * @returns  the result of the test, either pass/fail as reported by @b test
 *           or else segfault/other as captured by libgrading
 */
template<class T>
TestResult RunTest(std::function<TestResult (T&)> test, T& output,
                   std::string name = "<unnamed test>",
                   time_t timeout = 0,
                   std::ostream& errorStream = std::cerr)
{
	std::unique_ptr<SharedMemory> mem(MapSharedData(sizeof(T)));
	T *testOutput = static_cast<T*>(mem->rawPointer());

	TestResult result =
		RunTest([&]() { return test(*testOutput); }, name,
		        timeout, errorStream);

	output = *testOutput;
	return result;
}


/**
 * Run a test with input and output.
 *
 * @param    t       a test function that takes an input expectation,
 *                   copies out an output value and returns a @ref TestResult
 * @param    expect  our test expectation (inputs, expected output, etc.)
 * @param    output  the output value produced by @b test
 *                   (if it executes normally, e.g., without segfaulting)
 * @param    name    a developer-friendly name
 * @param    timeout the number of seconds to let the test run
 *                   (default 0, meaning "no timeout, run forever")
 * @param    errorStream  where to write messages (e.g., "expected X, got Y")
 *
 * @returns  the result of the test, either pass/fail as reported by @b test
 *           or else segfault/other as captured by libgrading
 */
template<class Expectation, class Output>
TestResult RunTest(std::function<TestResult (const Expectation&, Output&)> t,
                   const Expectation& expect, Output& output,
                   std::string name = "<unnamed test>", time_t timeout = 0,
                   std::ostream& errorStream = std::cerr)
{
	using std::placeholders::_1;
	return RunTest<Output>(
		std::bind(t, expect, _1), output, name, timeout, errorStream);
}

} // namespace grading

#endif

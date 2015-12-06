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
#include <unordered_set>
#include <vector>


//! Container for all libgrading names.
namespace grading {


class Test;
class TestBuilder;
class TestSuite;


//! The result of running one test.
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


/**
 * Ways that we can run tests.
 *
 * We can select among these at run-time with the command-line argument
 * `--strategy=inline|separated|sandboxed`.
 */
enum class TestRunStrategy
{
	Inline,      //!< In the same process, in the current call stack.
	Separated,   //!< In separate but unsandboxed processes.
	Sandboxed,   //!< In a separate, sandboxed process (if supported).
};


/**
 * A collection of tests that we can run.
 */
class TestSuite
{
	public:
	//! Default constructor: create an empty test suite.
	TestSuite();

	//! Construct a TestSuite from a brace-enclosed list of Test objects.
	TestSuite(std::initializer_list<Test>);

	/**
	 * Start creating a test with a given name.
	 *
	 * When the @ref TestBuilder goes out of scope, it will add a complete
	 * @ref Test to this suite.
	 *
	 * @param   name       user-meaningful test name
	 *
	 * @pre     @b name should not be empty
	 */
	TestBuilder add(std::string name);

	/**
	 * Add an already-complete @ref Test to this suite.
	 */
	TestSuite& add(Test);

	//! The total weight of all tests in the suite.
	unsigned int totalWeight() const;

	//! Summary statistics about the execution of a TestSuite.
	struct Statistics
	{
		unsigned int passed;    //!< tests that passed (unweighted)
		unsigned int failed;    //!< tests that failed (unweighted)
		float score;            //!< weighted (passed/total) score
		unsigned int total;     //!< total test count (unweighted)
	};

	/**
	 * Run all tests, using command-line arguments to guide the
	 * testing strategy, timeouts, etc.
	 *
	 * @returns  summary statistics about the suite run
	 */
	Statistics Run(int argc, char *argv[]) const;

	private:
	std::vector<Test> tests_;
};


//! A closure that wraps a single test case.
typedef std::function<void ()> TestClosure;


/**
 * An object used to construct a complete @ref Test.
 */
class TestBuilder
{
	public:
	//! Construct a builder for a named test within a @ref TestSuite.
	TestBuilder(TestSuite&, std::string name);
	~TestBuilder();

	//! Set description (which will be printed when run in verbose mode).
	TestBuilder& description(std::string);

	//! Set description (which will be printed when run in verbose mode).
	TestBuilder& desc(std::string d) { return description(d); }

	//! Set the test timeout (0 means "run forever").
	TestBuilder& timeout(time_t);

	/**
	 * Set the weight accorded to a test.
	 *
	 * Unlike many unit testing libraries, this library is intended for
	 * use in automatic grading software. In such software, we may wish
	 * to assign different amounts of weight to different tests for the
	 * purpose of fair and equitable grading.
	 */
	TestBuilder& weight(unsigned int);

	private:
	TestSuite& suite_;

	const std::string name_;
	std::string description_;
	TestClosure test_;
	time_t timeout_;
	unsigned int weight_;
};


/**
 * A single, completely-initialized test.
 */
class Test
{
	public:
	/**
	 * A set of arbitrary tags that can be used when selecting tests.
	 *
	 * Tag-informed test running isn't implemented yet, but we want to be
	 * able to select, e.g., only the short tests or only the bonus tests.
	 */
	typedef std::unordered_set<std::string> TagSet;

	/**
	 * Standard constructor.
	 *
	 * Takes a @ref TestClosure that represents the test we are
	 * actually going to run.
	 */
	Test(std::string name, std::string description, TestClosure,
	     time_t timeout = 0, unsigned int weight = 1,
	     TagSet tags = TagSet());

	/**
	 * Function-plus-expectation constructor.
	 *
	 * Takes a single-argument std::function and a single value
	 * to pass into that function (e.g., a common-to-all-tests
	 * `TestStudentFunction(const Expectation&)` and an `Expectation`
	 * value that describes a particular test case).
	 */
	template<class Expectation>
	Test(std::string name, std::string description,
	     std::function<void (const Expectation&)> fn,
	     Expectation e, time_t timeout = 0, unsigned int weight = 1)
		: Test(name, description, std::bind(fn, e), timeout, weight)
	{
	}

	/**
	 * Function-pointer constructor.
	 *
	 * Takes a pointer to a single-argument function and a single value
	 * to pass into that function (e.g., a common-to-all-tests
	 * `TestStudentFunction(const Expectation&)` and an `Expectation`
	 * value that describes a particular test case).
	 */
	template<class Expectation>
	Test(std::string name, std::string description,
	     void (*fn)(const Expectation&),
	     Expectation e, time_t timeout = 0, unsigned int weight = 1)
		: Test(name, description, std::bind(fn, e), timeout, weight)
	{
	}

	//! User-meaningful test name (ideally a single line or less).
	std::string name() const { return name_; }

	//! A longer test description. May contain newlines.
	std::string description() const { return description_; }

	//! Maximum length of time this test should take (or 0 for unlimited).
	time_t timeout() const { return timeout_; }

	//! How much weight to place on this test when calculating final score.
	unsigned int weight() const { return weight_; }

	/**
	 * Run this test.
	 *
	 * @param  strategy     how to run the test (e.g., sandboxed)
	 * @param  timeout      how long to wait for completion (0 = forever)
	 */
	TestResult Run(TestRunStrategy strategy, time_t timeout = 0) const;


	private:
	const std::string name_;
	const std::string description_;
	const TestClosure test_;
	const time_t timeout_;
	const unsigned int weight_;
	const TagSet tags_;

	friend class TestBuilder;
};


/**
 * The result of executing a `CheckSomething()` function.
 */
class CheckResult
{
	public:
	//! "All's-well" constructor (i.e., the check passed).
	CheckResult();

	//! Constructor that takes a simple error message.
	CheckResult(std::string message);

	//! Constructor that takes an expected and actual value.
	CheckResult(std::string exected, std::string actual);

	CheckResult(const CheckResult&) = delete;

	//! Move constructor. Steal error result from a temporary CheckResult.
	CheckResult(CheckResult&&);

	~CheckResult();

	//! Add further error details to this result, should it be a failure.
	CheckResult& operator << (const std::vector<std::string>&);

	//! Add further error details to this result, should it be a failure.
	template<class T>
	CheckResult& operator << (const T& x)
	{
		message_ << x;
		return *this;
	}

	//! Whether or not the check result is erroneous.
	bool error() const { return reportError_; }

	/**
	 * Cancel the error: the result is actually ok (e.g., because of a
	 * logical OR) or else the error's ownership is being transferred
	 * to another CheckResult.
	 */
	void cancel() { reportError_ = false; }

	//! Value test expected to see (user-readable representation).
	std::string actual() const { return actual_; }

	//! Actual value that was seen (user-readable representation).
	std::string expected() const { return expected_; }

	//! A message to display if the check fails.
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

//! Fail the current test.
CheckResult Fail(std::string message);


//! Output a human-readable representation of a @ref TestResult.
std::ostream& operator << (std::ostream&, TestResult);


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

} // namespace grading

#endif

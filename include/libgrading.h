/*!
 * @file      libgrading.h
 * @brief     A library for grading C- and C++-based assignments.
 *
 * @author    Jonathan Anderson <jonathan.anderson@mun.ca>
 * @copyright (c) 2014 Jonathan Anderson. All rights reserved.
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
#include <memory>
#include <string>

namespace grading {

enum class TestResult : char
{
	Pass,
	Fail,
	Segfault,
	OtherError
};

std::ostream& operator << (std::ostream&, TestResult);


/**
 * Run a test closure in a separate process, capturing segmentation faults
 * and other errors that lead to termination.
 */
TestResult RunInChild(std::function<TestResult ()> test);


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
	virtual void* rawPointer() const = 0;
};


//! Map data into the address space that can be shared with other processes.
std::unique_ptr<SharedMemory> MapSharedData(size_t size);




/**
 * Run a test that takes no explicit input.
 *
 * @param    test    a @ref std::function that copies out an output value
 *                   and returns a @ref TestResult
 * @param    output  the output value produced by @b test
 *                   (if it executes normally, e.g., without segfaulting)
 *
 * @returns  the result of the test, either pass/fail as reported by @b test
 *           or else segfault/other as captured by @ref RunInChild.
 */
template<class T>
TestResult TestInChild(std::function<TestResult (T&)> test, T& output)
{
	std::unique_ptr<SharedMemory> mem(MapSharedData(sizeof(T)));
	T *testOutput = static_cast<T*>(mem->rawPointer());

	TestResult result = RunInChild([&]() { return test(*testOutput); });

	output = *testOutput;
	return result;
}

//! Run a test that takes an input, copies out a value and returns a TestResult.
template<class Input, class Output>
TestResult TestInChild(std::function<TestResult (const Input&, Output&)> test,
                       const Input& expectation, Output& output)
{
	using std::placeholders::_1;
	return TestInChild<Output>(std::bind(test, expectation, _1), output);
}

} // namespace grading

#endif

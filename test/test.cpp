/*!
 * @file      test.cpp
 * @brief     Tests for libgrading.
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

#include <libgrading.h>
#include <iostream>

using grading::TestResult;
using namespace std;


//! Describes a test case in the problem domain.
struct Expectation
{
	// These fields describe a normal test expectation:
	const string name;
	const int *values;
	const unsigned int index;
	const int value;

	//! This field is used to test the tester:
	const TestResult expectedTestResult;
};


//! This function contains a logical error or two.
int FunctionUnderTest(const int v[], unsigned int index)
{
	return v[index * 10000000 + 1];
}


const int EvenNumbers[] = { 2, 4, 6, 8, 10 };
const int Fibbonacci[] = { 1, 1, 2, 3, 5 };


//! Test vectors to pass in to the (erroneous) @ref FunctionUnderTest.
const Expectation tests[] =
{
	{ "first element", Fibbonacci, 0, 1, TestResult::Pass },
	{ "wrong element", EvenNumbers, 0, 2, TestResult::Fail },
	{ "out of bounds", EvenNumbers, 4, 10, TestResult::Segfault },
};


int main(int /*argc*/, char* /*argv*/[])
{
	size_t passes = 0;
	size_t failures = 0;

	for (const Expectation& i : tests)
	{
		cout << "Running test '" << i.name << "'... ";
		cout.flush();

		int value;
		std::function<TestResult (int&)> test = [&](int& output)
		{
			output = FunctionUnderTest(i.values, i.index);
			if (output == i.value)
				return TestResult::Pass;
			else
				return TestResult::Fail;
		};

		const TestResult result = grading::RunTest(test, value);

		if (result == i.expectedTestResult)
		{
			passes++;
			cout << "success.\n";
		}
		else
		{
			failures++;
			cout
				<< "\n  ERROR: expected "
				<< i.expectedTestResult
				<< ", got " << result
				<< "\n"
				;
		}
	}

	cout
		<< "\n"
		<< "Passed " << passes << " out of "
		<< (sizeof(tests) / sizeof(tests[0])) << " tests.\n"
		;

	return 0;
}

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

#include <algorithm>
#include <iostream>

using namespace grading;
using namespace std;


//! Describes a test case in the problem domain.
struct Expectation
{
	// These fields describe a normal test expectation:
	const int *values;
	const unsigned int index;
	const int value;

	//! This field is used to test the tester:
	const TestExitStatus expectedStatus;
};


//! This function contains a logical error or two.
int FunctionUnderTest(const int v[], unsigned int index)
{
	cout << "This is stdout from FunctionUnderTest(v, " << index << ").\n";
	cerr << "This is stderr from FunctionUnderTest(v, " << index << ").\n";

	return v[index * 10000000 + 1];
}

void TestStudentFn(const Expectation& e)
{
	int output = FunctionUnderTest(e.values, e.index);
	CheckInt(e.value, output);
};


const int EvenNumbers[] = { 2, 4, 6, 8, 10 };
const int Fibbonacci[] = { 1, 1, 2, 3, 5 };


const TestSuite tests =
{
	{
		"should pass",
		" - correct expectation: the first element in the"
		" Fibonnacci sequence is 1\n"
		" - FunctionUnderTest will return the correct value\n"
		" - this test should pass",
		TestStudentFn,
		{ Fibbonacci, 0, 1, TestExitStatus::Pass },
	},

	{
		"should fail",
		" - incorrect expectation: the first even number is 2\n"
		" - FunctionUnderTest will return the wrong number\n"
		" - this test should fail",
		TestStudentFn,
		{ EvenNumbers, 0, 2, TestExitStatus::Fail },
		0, 10
	},

	{
		"should segfault",
		" - test deferences nullptr\n"
		" - FunctionUnderTest will segfault\n"
		" - this test's segfault should be contained",
		[]()
		{
			*static_cast<volatile double*>(nullptr);
		},
	},

	{
		"should timeout",
		" - test times out\n"
		" - the timeout should be interrupted after 1s",
		[]()
		{
			while (true) {}
		},
		1
	},
};


int main(int argc, char* argv[])
{
	tests.Run(argc, argv);
	return 0;
}

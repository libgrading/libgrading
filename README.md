# libgrading

This is a simple library for grading C- and C++-language assignments.
It runs each test case in a child process in order to capture common
programming errors such as infinite loops and segmentation faults.

## Get it

libgrading is
[hosted on GitHub](https://github.com/trombonehero/libgrading).
You can get the most recent version with either of the following commands:

~~~sh
$ git clone https://github.com/trombonehero/libgrading.git
$ svn checkout https://github.com/trombonehero/libgrading
~~~

At some point, I might also create releases.


## Build it

First, install [libdistance](http://monkey.org/~jose/software/libdistance/).
Then:

~~~sh
$ mkdir build
$ cd build
$ cmake ..       # or cmake -G Ninja ..
$ make           # or ninja
~~~

## Use it

~~~cpp
#include <libgrading.h>
using namespace grading;
using namespace std;


//
// First, you need to describe some test cases *in the problem domain*:
// we don't need any libgrading boilerplate or TEST_CASE syntax.
//
struct AdditionExpectation
{
	int x;
	int y;
	int sum;
};

const AdditionExpectation tests[] =
{
	{ 1, 1, 2 },
	{ 1, 2, 3 },
	{ 2, 3, 5 },
	{ 3, 5, 8 },

	{ 0, 0, 0 },
	{ 0, -2, -2 },
	{ -4, 2, -2 },

	// ...
};


//
// Next, define a function (or a lambda in a std::function) that takes
// a single input expectation, runs the code under test and (optionally)
// copies out a value by reference.
//
// It will be run in a separate process in case the code under test causes
// a segmentation fault or other termination error.
//
// This function needs to return a grading::TestResult.
//
TestResult TestSubmittedFunction(const AdditionExpectation& expected, int& sum)
{
	// In a real test suite, you'd link against submitted code.
	// For this demo, we'll use a lambda.
	auto studentFunction = [](int x, int y) { return x + y + 1; };

	sum = studentFunction(expected.x, expected.y);
	CheckInt(expected.sum, sum)
		<< "some more detail to be output if this check fails";

	return TestResult::Pass;
}


//
// Finally, in your main function, call grading::RunTest() for each
// of your test cases and accumulate the results however you like.
//
// Future versions of libgrading might include a test runner if I need one.
//
int main(int argc, char *argv[])
{
	constexpr size_t testCount = sizeof(tests) / sizeof(tests[0]);
	constexpr size_t testTimeout = 5;    // kill tests after 5 s
	size_t failures = 0;

	for (const Expectation& i : tests)
	{
		int sum;
		const TestResult result =
			RunTest(TestSubmittedFunction, i, sum, testTimeout);

		if (result != TestResult::Pass)
		{
			failures++;
			// maybe save the failure information somewhere?
			// maybe output an error message?
		}
	}

	cout
		<< "\n"
		<< "Passed " << (testCount - failures) << " out of "
		<< testCount << " tests.\n"
		;

	return 0;
}
~~~

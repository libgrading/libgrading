# libgrading

This is a simple library for grading C- and C++-language assignments.
It runs each test case in a child process in order to capture common
programming errors such as segmentation faults.

## Usage

```cpp
#include <libgrading.h>

using grading::TestResult;
using namespace std;


//! Describes a test case in the problem domain.
struct AdditionExpectation
{
	int x;
	int y;
	int sum;
};


//! Test vectors to pass in to the (erroneous) @ref FunctionUnderTest.
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


TestResult TestSubmittedFunction(const AdditionExpectation& expected, int& value)
{
	// In a real test suite, you'd link against submitted code.
	// For this demo, we'll use a lambda.
	auto studentFunction = [](int x, int y) { return x + y + 1; };

	if (studentFunction(expected.x, expected.y) == expected.value)
		return TestResult::Pass;
	else
		return TestResult::Fail;

	// libgrading will take care of the other cases
}


int main(int argc, char *argv[])
{
	constexpr size_t testCount = sizeof(tests) / sizeof(tests[0]);
	size_t failures = 0;

	for (const Expectation& i : tests)
	{
		int sum;
		const TestResult result =
			grading::TestInChild(TestSubmittedFunction, i, sum);

		if (result != i.expectedTestResult)
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
```

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
// One way to define tests is to define a domain-relevant expectation
// (inputs and outputs) and a function that will check that expectation:
//
struct AdditionExpectation
{
	int x;
	int y;
	int sum;
};

void TestStudentFn(const AdditionExpectation& expected)
{
	// In a real test suite, you'd link against submitted code.
	// For this demo, we'll use a lambda.
	auto studentFunction = [](int x, int y) { return x + y + 1; };

	int sum = studentFunction(expected.x, expected.y);
	CheckInt(expected.sum, sum)
		<< "some more detail to be output if this check fails";
}


//
// You can define a grading::TestSuite declaratively, if you like such things
// (vs. using the TestBuilder class, as inside the `main` function below):
//
const TestSuite testClosures =
{
	// This is an example of a Test derived from an expectation and an
	// evaluation function.
	{
		"simple addition test",
		" - first part of the long description\n"
		" - second part of the long description",
		TestStudentFn,
		{ 2, 2, 5 },
		0,	// timeout: optional, 0 (the default) means forever
		1,	// weight to give this test (optional, default 1)
	},

	// Tests can also be created from self-contained test closures:
	{
		"test name",
		"long description ...",
		[]()
		{
			int x = foo();
			CheckInt(42, x)
				<< "the result of calling foo() should be 42"
				;

			// segmentation faults will be caught and handled
			// properly when running with --strategy=separated
			// or --strategy=sandboxed (the default)
			double *x = nullptr;
			double y = *x;
		},
	},

	// ...
};


//
// You need to write a `main` function to actually run the tests,
// but it can be very simple indeed:
//
int main(int argc, char *argv[])
{
	const TestSuite::Statistics stats = tests.Run(argc, argv);

	// optionally do something with the stats, such as:
	cout << "Grade: " << stats.score << endl;

	return 0;
}
~~~

/*!
 * @file      TestSuite.cpp
 * @brief     Definitions of @ref grading::TestSuite.
 *
 * @author    Jonathan Anderson <jonathan.anderson@mun.ca>
 * @copyright (c) 2015 Jonathan Anderson. All rights reserved.
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

#include "private.h"
#include <libgrading.h>
#include <cassert>
using namespace grading;
using namespace std;


TestSuite::TestSuite()
{
}


TestSuite::TestSuite(std::initializer_list<Test> tests)
	: tests_(tests)
{
}


TestBuilder TestSuite::add(string name)
{
	assert(not name.empty());

	return TestBuilder(*this, name);
}


TestSuite& TestSuite::add(Test test)
{
	tests_.push_back(move(test));
	return *this;
}


unsigned int TestSuite::totalWeight() const
{
	unsigned int total = 0;

	for (const Test& test : tests_)
	{
		total += test.weight();
	}

	return total;
}


TestSuite::Statistics TestSuite::Run(int argc, char *argv[]) const
{
	Statistics stats = { 0, 0, 0, 0 };

	const Arguments args = Arguments::Parse(argc, argv);
	if (args.error or args.help or args.skip)
	{
		return stats;
	}

	for (const Test& test : tests_)
	{
		const static string line(80, '-');
		const static string doubleLine(80, '=');

		if (args.verbose)
		{
			cout
				<< doubleLine << "\n"
				<< "Running test: '" << test.name() << "'.\n"
				<< "Description:\n" << test.description()
				<< "\n" << line << "\n"
				;
		}
		else
		{
			cout << "Running test '" << test.name() << "'... ";
		}

		stats.total++;

		TestResult result = test.Run(args.runStrategy, args.timeout);

		if (args.verbose)
		{
			cout
				<< line << "\n"
				<< "Test '" << test.name() << "' complete: "
				<< result << ".\n"
				<< doubleLine << "\n\n"
				;
		}
		else
		{
			cout << result << "." << std::endl;
		}

		if (result == TestResult::Pass)
		{
			stats.passed++;
			stats.score += test.weight();
		}
		else
		{
			stats.failed++;
		}
	}

	stats.score /= totalWeight();
	return stats;
}

/*!
 * @file      TestSuite.cpp
 * @brief     Definitions of @ref grading::TestSuite.
 *
 * @author    Jonathan Anderson <jonathan.anderson@mun.ca>
 * @copyright (c) 2015, 2019, 2022 Jonathan Anderson. All rights reserved.
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


TestSuite::TestSuite(std::initializer_list<TestBuilder> builders)
{
	for (const TestBuilder &tb : builders)
	{
		tests_.push_back(tb.build());
	}
}


TestSuite& TestSuite::add(TestBuilder b)
{
	return this->add(b.build());
}


TestSuite& TestSuite::add(Test test)
{
	tests_.push_back(std::move(test));
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

	auto f = Formatter::Create(args.outputFormat, cout);

	for (const Test& test : tests_)
	{
		f->testBeginning(test);
		stats.total++;

		TestResult result = test.Run(args.runStrategy, args.timeout);

		f->testEnded(test, result);

		if (result.status == TestExitStatus::Pass)
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
	f->suiteComplete(*this, stats);

	return stats;
}

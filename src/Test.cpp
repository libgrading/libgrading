/*!
 * @file      Test.cpp
 * @brief     Definitions of @ref grading::Test.
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

#include <libgrading.h>
#include "private.h"

using namespace grading;
using std::string;


Test::Test(string name, string description, TestClosure test,
           time_t timeout, unsigned int weight, TagSet tags)
	: name_(name), description_(description), test_(test),
	  timeout_(timeout), weight_(weight), tags_(tags)
{
}


TestResult Test::Run(TestRunStrategy strategy, time_t timeout,
                     std::ostream& errorStream) const
{
	if (timeout == 0)
		timeout = timeout_;

	else if (timeout_ != 0)
		timeout = std::min(timeout, timeout_);


	switch (strategy)
	{
		case TestRunStrategy::Inline:
			test_();
			return TestResult::Pass;

		case TestRunStrategy::Separated:
			return ForkTest(test_, timeout, errorStream);

		case TestRunStrategy::Sandboxed:
		{
			TestClosure t = [this]()
			{
				EnterSandbox();
				test_();
			};

			return ForkTest(t, timeout, errorStream);
		}
	}
}


TestResult grading::RunInProcess(TestClosure test)
{
	try
	{
		test();
		return TestResult::Pass;
	}
	catch (const std::exception& e)
	{
		std::cerr
			<< typeid(e).name() << ": "
			<< e.what() << std::endl
			;

		return TestResult::UncaughtException;
	}
	catch (int i)
	{
		std::cerr << "caught int: " << i << std::endl;
		return TestResult::UncaughtException;
	}
	catch (const std::string& s)
	{
		std::cerr
			<< "caught string: '" << s << "'"
			<< std::endl;

		return TestResult::UncaughtException;
	}
	catch (...)
	{
		std::cerr << "uncaught exception!" << std::endl;
		return TestResult::UncaughtException;
	}
}

/*!
 * @file      TestExitStatus.cpp
 * @brief     Definitions of @ref grading::TestExitStatus functions.
 *
 * @author    Jonathan Anderson <jonathan.anderson@mun.ca>
 * @copyright (c) 2014-2015 Jonathan Anderson. All rights reserved.
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
#include <ostream>


std::ostream& grading::operator << (std::ostream& out, TestExitStatus result)
{
	switch (result)
	{
		case TestExitStatus::Pass:       out << "passed"; break;
		case TestExitStatus::Fail:       out << "failed"; break;
		case TestExitStatus::Abort:      out << "aborted"; break;
		case TestExitStatus::Segfault:
			out << "segmentation fault";
			break;

		case TestExitStatus::Timeout:
			out << "timeout";
			break;

		case TestExitStatus::UncaughtException:
			out << "uncaught exception";
			break;

		case TestExitStatus::OtherError:
			out << "unknown test error";
			break;
	}

	return out;
}

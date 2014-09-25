/*!
 * @file      TestResult.cpp
 * @brief     Definitions of @ref grading::TestResult functions.
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
#include <ostream>


std::ostream& grading::operator << (std::ostream& out, TestResult result)
{
	switch (result)
	{
		case TestResult::Pass:       out << "Pass"; break;
		case TestResult::Fail:       out << "Fail"; break;
		case TestResult::Segfault:   out << "Segfault"; break;
		case TestResult::OtherError: out << "OtherError"; break;
	}

	return out;
}

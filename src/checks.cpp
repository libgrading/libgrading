/*!
 * @file      checks.cpp
 * @brief     Implementation of various expectation checks.
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

#include <cassert>
#include <cmath>
#include <functional>

using std::string;
using std::to_string;


namespace grading {

CheckResult::CheckResult()
	: reportError_(false), expected_(""), actual_("")
{
}

CheckResult::CheckResult(string message)
	: reportError_(true), expected_(""), actual_(message)
{
}

CheckResult::CheckResult(string expected, string actual)
	: reportError_(true), expected_(expected), actual_(actual)
{
}

CheckResult::CheckResult(CheckResult&& other)
	: reportError_(other.reportError_),
	  expected_(other.expected_), actual_(other.actual_),
	  message_(std::move(other.message_))
{
	other.reportError_ = false;
}


//
// NOTE: CheckResult::~CheckResult() is implementation-specific and is
//       implemented in posix.cpp, etc.
//


CheckResult operator && (CheckResult&& x, CheckResult&& y)
{
	if (not x.error() and not y.error())
	{
		x.cancel();
		y.cancel();

		return CheckResult();
	}

	if (not x.error())
		return std::move(y);

	else if (not y.error())
		return std::move(x);

	const string exp = "(" + x.expected() + " and " + y.expected() + ")";
	const string actual = 
		(x.actual() == y.actual())
		? x.actual()
		: "(" + x.actual() + " or " + y.actual() + ")"
		;

	x.cancel();
	y.cancel();

	return CheckResult(exp, actual);
}


CheckResult operator || (CheckResult&& x, CheckResult&& y)
{
	if (not x.error() or not y.error())
	{
		x.cancel();
		y.cancel();

		return CheckResult();
	}

	
	const string exp = "(" + x.expected() + " or " + y.expected() + ")";
	const string actual = 
		(x.actual() == y.actual())
		? x.actual()
		: "(" + x.actual() + " or " + y.actual() + ")"
		;

	x.cancel();
	y.cancel();

	return CheckResult(exp, actual);
}


CheckResult& CheckResult::operator << (const std::vector<std::string>& v)
{
	message_ << "[";

	for (const string& s : v)
		message_ << " " << s;

	message_ << " ]";

	return *this;
}


//
// Checks for tests:
//

CheckResult Check(bool condition, string description)
{
	if (condition)
		return CheckResult();

	return CheckResult(description);
}

CheckResult CheckInt(int expected, int actual)
{
	if (expected == actual)
		return CheckResult();

	return CheckResult(to_string(expected), to_string(actual));
}

CheckResult CheckFloat(double exp, double actual, double tolerance)
{
	const double error = fabs(actual - exp);

	if (error < tolerance)
		return CheckResult();

	const double relativeTolerance = exp * tolerance;

	if (error < relativeTolerance)
		return CheckResult();

	return CheckResult(to_string(exp), to_string(actual));
}

CheckResult CheckString(string expected, string actual)
{
	if (expected == actual)
		return CheckResult();

	return CheckResult(expected, actual);
}

} // namespace grading

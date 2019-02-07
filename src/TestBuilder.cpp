/*!
 * @file      TestBuilder.cpp
 * @brief     Definitions of @ref grading::TestBuilder.
 *
 * @author    Jonathan Anderson <jonathan.anderson@mun.ca>
 * @copyright (c) 2015 Jonathan Anderson. All rights reserved.
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
using namespace grading;
using std::string;


TestBuilder::TestBuilder(TestSuite& suite, string name)
	: suite_(suite), name_(name), timeout_(0), weight_(1)
{
}


TestBuilder::~TestBuilder()
{
	suite_.add(Test(name_, description_, test_, timeout_, weight_));
}


TestBuilder& TestBuilder::description(string d)
{
	description_ = d;
	return *this;
}

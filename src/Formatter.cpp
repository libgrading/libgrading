/*!
 * @file      Formatter.cpp
 * @brief     Definitions of @ref grading::Formatter.
 *
 * @author    Jonathan Anderson <jonathan.anderson@mun.ca>
 * @copyright (c) 2019 Jonathan Anderson. All rights reserved.
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


namespace {

class BriefFormatter : public Formatter
{
public:
	BriefFormatter(std::ostream &os) : Formatter(os) {}

	virtual void testBeginning(const Test &test) override;
	virtual void testEnded(const Test &test, const TestResult&) override;
	virtual void suiteComplete(const TestSuite&,
	                           TestSuite::Statistics) override;
};

class VerboseFormatter : public Formatter
{
public:
	VerboseFormatter(std::ostream &os);

	virtual void testBeginning(const Test &test) override;
	virtual void testEnded(const Test &test, const TestResult&) override;
	virtual void suiteComplete(const TestSuite&,
	                           TestSuite::Statistics) override;

private:
	const string line_;
	const string doubleLine_;
};

} // anonymous namespace


Formatter::Formatter(ostream &os)
	: out_(os)
{
}

Formatter::~Formatter()
{
}

unique_ptr<Formatter> Formatter::Create(OutputFormat format, ostream &out)
{
	switch (format)
	{
	case OutputFormat::Brief:
		return unique_ptr<Formatter>(new BriefFormatter(out));

	case OutputFormat::Verbose:
		return unique_ptr<Formatter>(new VerboseFormatter(out));
	}

	assert(false && "unreachable");
}


void BriefFormatter::testBeginning(const Test &test)
{
	out_ << "Running test '" << test.name() << "'... ";
}

void BriefFormatter::testEnded(const Test &test, const TestResult &result)
{
	out_ << result.status << "." << std::endl;
}

void BriefFormatter::suiteComplete(const TestSuite&,
                                   TestSuite::Statistics stats)
{
	if (stats.total > 0)
	{
		out_
			<< "Passed " << stats.passed << " out of "
			<< stats.total << " tests\n"
			;
	}
}


VerboseFormatter::VerboseFormatter(std::ostream &os)
	: Formatter(os), line_(80, '-'), doubleLine_(80, '=')
{
}

void VerboseFormatter::testBeginning(const Test &test)
{
	out_
		<< doubleLine_ << "\n"
		<< "Running test: '" << test.name() << "'.\n"
		<< "Description:\n" << test.description() << "\n"
		;
}

void VerboseFormatter::testEnded(const Test &test, const TestResult &result)
{
	out_ << "Result: " << result.status << "\n";

	if (not result.output.empty())
	{
		out_
			<< line_ << "\n"
			<< "Standard output (stdout/cout):\n"
			<< line_ << "\n"
			<< result.output
			<< line_ << "\n"
			;
	}

	if (not result.errorOutput.empty())
	{
		out_
			<< line_ << "\n"
			<< "Error output (stderr/cerr):\n"
			<< line_ << "\n"
			<< result.errorOutput
			<< line_ << "\n"
			;
	}

	out_ << doubleLine_ << "\n\n";
}

void VerboseFormatter::suiteComplete(const TestSuite&,
                                     TestSuite::Statistics stats)
{
	if (stats.total > 0)
	{
		out_
			<< "Passed " << stats.passed << " out of "
			<< stats.total << " tests\n"
			;
	}
}

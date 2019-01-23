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

class GradescopeFormatter : public Formatter
{
public:
	GradescopeFormatter(std::ostream &os) : Formatter(os) {}

	virtual void testEnded(const Test &test, const TestResult&) override;
	virtual void suiteComplete(const TestSuite&,
	                           TestSuite::Statistics) override;

private:
	struct Result
	{
		const std::string name;
		const TestExitStatus status;
		const std::string output;
	};

	std::vector<Result> testResults;
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

	case OutputFormat::Gradescope:
		return unique_ptr<Formatter>(new GradescopeFormatter(out));

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


void GradescopeFormatter::testEnded(const Test &test, const TestResult &result)
{
	string output = "Test description:\\n" + test.description();
	output += "\n\nTest output:\n";
	output += result.output + result.errorOutput;

	// Escape newline characters
	ssize_t pos = 0;
	do
	{
		pos = output.find('\n', pos);
		if (pos != string::npos)
		{
			output.replace(pos, 1, "\\n");
		}
	}
	while (pos != string::npos);

	testResults.push_back({
		.name = test.name(),
		.status = result.status,
		.output = std::move(output),
	});
}

void GradescopeFormatter::suiteComplete(const TestSuite&,
                                        TestSuite::Statistics stats)
{
	out_ << "{";
	out_ << "\"tests\":[";

	// Sigh, JSON with your lack of support for trailing commas...
	for (int i = 0; i < testResults.size(); i++)
	{
		const Result &r = testResults[i];

		out_
			<< "{"

			<< "\"name\":\"" << r.name << "\","

			<< "\"score\":"
			<< ((r.status == TestExitStatus::Pass) ? 1 : 0)
			<< ","

			<< "\"visibility\":"
			<< "\"after_published\""
			<< ","

			<< "\"output\":\""
			<< r.output
			<< "\""

			<< "}"
			;


		if ((i + 1) < testResults.size())
		{
			out_ << ",";
		}
	}

	out_ << "]";
	out_ << "}\n";
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

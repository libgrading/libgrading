/*!
 * @file      Arguments.cpp
 * @brief     Implementation of @ref grading::Arguments.
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

#include <optionparser.h>

#include <vector>

using namespace grading;
using std::vector;


//! Constants used to identify arguments for optionparser.
enum Options
{
	UNKNOWN,
	HELP,
	OUTPUT_FORMAT,
	SKIP_TESTS,
	RUN_STRATEGY,
	TIMEOUT,
};

//! Check that a required argument has been passed.
static option::ArgStatus Required(const option::Option& option, bool msg)
{
	if (option.arg != 0)
		return option::ARG_OK;

	if (msg)
		std::cerr << option.name << " requires an argument\n";

	return option::ARG_ILLEGAL;
}

//! Usage strings for command-line arguments.
const option::Descriptor usage[] =
{
	{
		UNKNOWN, 0, "" , "", option::Arg::None,
		"USAGE: <test-executable> [options]\n\n"
		"Options:"
	},
	{
		HELP, 0,
		"h", "help",
		option::Arg::None,
		"  -h, --help          Print usage and exit."
	},
	{
		OUTPUT_FORMAT, 0,
		"f", "format",
		Required,
		"  -f, --format        Output format"
		" (brief, gradescope, verbose)."
	},
	{
		SKIP_TESTS, 0,
		"s", "skip",
		option::Arg::None,
		"  -s, --skip-tests    Skip test execution (e.g., for build testing.)"
	},
	{
		RUN_STRATEGY, 0,
		"r", "run-strategy",
		Required,
		"  -r, --run-strategy  Strategy for running tests"
		" (inline, separated, sandboxed)."
	},
	{
		TIMEOUT, 0,
		"t", "timeout",
		Required,
		"  -t, --timeout       Kill tests after n seconds."
	},
	{0,0,0,0,0,0}
};


Arguments Arguments::Parse(int argc, char *argv[])
{
	argc -= 1;
	argv += 1;

	option::Stats stats(usage, argc, argv);
	vector<option::Option> options(stats.options_max);
	vector<option::Option> buffer(stats.buffer_max);

	option::Parser parse(usage, argc, argv,
	                     options.data(), buffer.data());

	if (parse.error())
		return Arguments { .error = true };

	if (options[HELP])
	{
		option::printUsage(std::cerr, usage);
		return Arguments { .error = false, .help = true };
	}

	const bool skip = options[SKIP_TESTS];

	OutputFormat format = OutputFormat::Brief;
	if (options[OUTPUT_FORMAT])
	{
		const std::string arg = options[OUTPUT_FORMAT].arg;

		if (arg == "brief")
		{
			format = OutputFormat::Brief;
		}
		else if (arg == "gradescope")
		{
			format = OutputFormat::Gradescope;
		}
		else if (arg == "verbose")
		{
			format = OutputFormat::Verbose;
		}
		else
		{
			std::cerr
				<< "Invalid --format: '" << arg << "'\n"
				"Valid options: brief, gradescope, verbose\n"
				;

			return Arguments { .error = true };
		}
	}

	TestRunStrategy strategy = TestRunStrategy::Sandboxed;
	if (options[RUN_STRATEGY])
	{
		const std::string strategyArg = options[RUN_STRATEGY].arg;

		if (strategyArg == "inline")
		{
			strategy = TestRunStrategy::Inline;
		}
		else if (strategyArg == "separated")
		{
			strategy = TestRunStrategy::Separated;
		}
		else if (strategyArg == "sandboxed")
		{
			strategy = TestRunStrategy::Sandboxed;
		}
		else
		{
			std::cerr
				<< "Invalid --strategy: '" << strategyArg << "'"
				"\n(valid strategies: "
				"inline, separated, sandboxed)\n"
				;

			return Arguments { .error = true };
		}
	}

	time_t timeout = 0;
	if (options[TIMEOUT])
	{
		const std::string arg = options[TIMEOUT].arg;
		timeout = std::atol(arg.c_str());
	}

	return Arguments
	{
		.error = false,
		.help = false,
		.outputFormat = format,
		.skip = skip,
		.runStrategy = strategy,
		.timeout = timeout,
	};
}

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
	VERBOSE,
	STRATEGY,
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
		VERBOSE, 0,
		"v", "verbose",
		option::Arg::None,
		"  -v, --verbose       Print details of every test."
	},
	{
		STRATEGY, 0,
		"s", "strategy",
		Required,
		"  -s, --strategy      Strategy for running tests"
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

	const bool verbose = options[VERBOSE];

	TestRunStrategy strategy = TestRunStrategy::Sandboxed;
	if (options[STRATEGY])
	{
		const std::string strategyArg = options[STRATEGY].arg;

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
		.verbose = verbose,
		.runStrategy = strategy,
		.timeout = timeout,
	};
}

/*!
 * @file      private.h
 * @brief     Internal header file for libgrading.
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

#ifndef LIBGRADING_PRIVATE_H
#define LIBGRADING_PRIVATE_H

#include <libgrading.h>


namespace grading {

/**
 * Parsed command-line arguments.
 */
struct Arguments
{
	//! Parse command-line arguments.
	static Arguments Parse(int argc, char *argv[]);

	//! There was an error parsing command-line arguments.
	const bool error;

	//! The `--help` argument was given.
	const bool help;

	//! The `--skip` argument was given.
	const bool skip;

	//! The `--verbose` argument was given.
	const bool verbose;

	//! The @ref TestRunStrategy chosen by the user (e.g., inline).
	const TestRunStrategy runStrategy;

	//! Maximum length of time to wait for any test.
	const time_t timeout;
};

/**
 * Enter unprivileged testing sandbox, if supported.
 */
void EnterSandbox();

/**
 * Run a test in another process.
 */
TestResult ForkTest(TestClosure test, time_t timeout);

/**
 * Run a test in the current process, catching all exceptions.
 */
TestResult RunInProcess(TestClosure test);

} // namespace grading

#endif

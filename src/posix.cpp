/*!
 * @file      posix.cpp
 * @brief     @internal POSIX implementation of
 *            @ref grading::CheckResult destructor,,
 *            @ref grading::MapSharedData, @ref grading::ForkTest and
 *            @ref grading::EnterSandbox.
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
#include "private.h"

#include <cassert>
#include <functional>

#include <sys/mman.h>
#include <sys/wait.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

using namespace grading;
using namespace std;


CheckResult::~CheckResult()
{
	if (reportError_)
	{
		cerr << "\nCheck failed: " << message_.str() << "\n";

		if (expected_.empty())
			cerr << "  " << actual_ << "\n";

		else
			cerr
				<< "  expected `" << expected_
				<< "`, got `" << actual_ << "`\n"
				;

		cerr
			<< "\n"
			;

		exit(static_cast<int>(TestExitStatus::Fail));
	}
}


static TestExitStatus ProcessChildStatus(int status)
{
	if (WIFEXITED(status))
		return static_cast<TestExitStatus>(WEXITSTATUS(status));

	if (WIFSIGNALED(status))
	{
		switch (WTERMSIG(status))
		{
			case SIGABRT:
			return TestExitStatus::Abort;

			case SIGSEGV:
			return TestExitStatus::Segfault;

			default:
			return TestExitStatus::OtherError;
		}
	}

	assert(false && "unhandled child exit mode");
}



/**
 * @brief A memory-mapped POSIX shared memory segment.
 */
class PosixSharedMemory : public SharedMemory
{
	public:
	/**
	 * Constructor.
	 *
	 * @param   fd       descriptor of (existing) shared memory segment
	 * @param   len      size of the shared memory [B]
	 * @param   rawPtr   pointer to the mmap'ed region
	 */
	PosixSharedMemory(int fd, size_t len, void *rawPtr)
		: shmfd(fd), length(len), ptr(rawPtr)
	{
	}

	~PosixSharedMemory()
	{
		munmap(ptr, length);
		close(shmfd);
	}

	//! Retrieve shared memory's file descriptor (POSIX-specific)
	int fd() const { return shmfd; }
	virtual void *rawPointer() const override { return ptr; }

	private:
	int shmfd;
	size_t length;
	void *ptr;
};


unique_ptr<SharedMemory> grading::MapSharedData(size_t len)
{
#if defined (__BSD_VISIBLE)
	int fd = shm_open(SHM_ANON, O_RDWR, 0600);
#else
	char tmpnameTemplate[] = "/tmp/libgrading.XXXXXX";
	int fd = mkstemp(tmpnameTemplate);
#endif

	if (fd < 0)
	{
		return nullptr;
	}

	if (ftruncate(fd, len) != 0)
	{
		close(fd);
		return nullptr;
	}

	void *map = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED)
	{
		return nullptr;
	}

	return unique_ptr<SharedMemory>(new PosixSharedMemory(fd, len, map));
}


TestResult grading::ForkTest(TestClosure test, time_t timeout)
{
	std::cout.flush();
	std::cerr.flush();
	std::clog.flush();

	fflush(stdout);
	fflush(stderr);

	auto out = MapSharedData(10 * 4096);
	if (not out)
	{
		return TestExitStatus::OtherError;
	}

	auto err = MapSharedData(10 * 4096);
	if (not err)
	{
		return TestExitStatus::OtherError;
	}

	pid_t child = fork();

	if (child == 0)
	{
		// Install shared file(s) as stdout and stderr
		int fd = dynamic_cast<const PosixSharedMemory&>(*out).fd();
		if (fd < 0 or dup2(fd, STDOUT_FILENO) < 0)
		{
			return TestExitStatus::OtherError;
		}

		fd = dynamic_cast<const PosixSharedMemory&>(*err).fd();
		if (fd < 0 or dup2(fd, STDERR_FILENO) < 0)
		{
			return TestExitStatus::OtherError;
		}

		TestExitStatus status = RunInProcess(test);
		exit(static_cast<int>(status));
	}
	else
	{
		int status;
		int options = (timeout ? WNOHANG : 0);

		time_t start = time(nullptr);

		while (true)
		{
			pid_t result = waitpid(child, &status, options);

			// Success: the child process has returned.
			if (result == child)
				break;

			// Error in waitpid()?
			if (result < 0)
			{
				assert(errno == EINTR);
				continue;
			}

			// Child process isn't finished yet.
			const time_t now = time(nullptr);
			if ((now - start) > timeout)
			{
				kill(child, SIGKILL);
				waitpid(child, &status, 0);
				return TestExitStatus::Timeout;
			}

			usleep(100);
		}

		return TestResult(ProcessChildStatus(status),
			string(static_cast<char*>(out->rawPointer())),
			string(static_cast<char*>(err->rawPointer())));
	}
}


#ifdef __FreeBSD__
#include <sys/param.h>

#if __FreeBSD_version >= 1001511
#include <sys/capsicum.h>
#else
#include <sys/capability.h>
#endif

#include <err.h>
#include <errno.h>
#include <sysexits.h>

void grading::EnterSandbox()
{
	if ((cap_enter() != 0) and (errno != ENOSYS))
		err(EX_OSERR, "Error in cap_enter()");
}
#else

#warning EnterSandbox() does nothing on the current platform
void grading::EnterSandbox()
{
}

#endif

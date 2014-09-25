/*!
 * @file      posix.cpp
 * @brief     POSIX implementation of @ref grading::MapSharedData and
 *            @ref grading::RunTest.
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
#include <functional>

#include <sys/mman.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

using namespace grading;
using namespace std;


static TestResult ProcessChildStatus(int status)
{
	if (WIFEXITED(status))
		return static_cast<TestResult>(WEXITSTATUS(status));

	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == 11)
			return TestResult::Segfault;

		else
			return TestResult::OtherError;
	}

	assert(false && "unhandled child exit mode");
}



class PosixSharedMemory : public SharedMemory
{
	public:
	PosixSharedMemory(int fd, size_t len, void *rawPtr)
		: shmfd(fd), length(len), ptr(rawPtr)
	{
	}

	~PosixSharedMemory()
	{
		munmap(ptr, length);
		close(shmfd);
	}

	virtual void *rawPointer() const override { return ptr; }

	private:
	int shmfd;
	size_t length;
	void *ptr;
};


unique_ptr<SharedMemory> grading::MapSharedData(size_t len)
{
	int shmfd = shm_open("shm.tmp", O_RDWR);
	void *map = mmap(0, len, PROT_READ | PROT_WRITE,
	                 MAP_ANON | MAP_SHARED, shmfd, 0);

	return unique_ptr<SharedMemory>(new PosixSharedMemory(shmfd, len, map));
}


TestResult grading::RunTest(function<TestResult ()> test)
{
	pid_t child = fork();

	if (child == 0)
	{
		TestResult result = test();
		exit(static_cast<int>(result));
	}
	else
	{
		int status;
		while (waitpid(child, &status, 0) < 0)
			if (errno != EINTR)
				err(-1, "unknown error in child process");

		return ProcessChildStatus(status);
	}
}

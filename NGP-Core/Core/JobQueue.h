/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

#include <vector>
#include <mutex>

struct JobQueue
{
	struct Job
	{
		void(*job_fn)(void*, u64, u64, u64, u64);
		void* instance; // For classes
		u64 arg1;
		u64 arg2;
		u64 arg3;
		u64 arg4;
	};

	void dispatch_jobs()
	{
		mutex.lock();

		for (auto& job : jobs)
		{
			job.job_fn(job.instance, job.arg1, job.arg2, job.arg3, job.arg4);
		}
		jobs.clear();

		mutex.unlock();
	}

	void add(const Job& new_job)
	{
		mutex.lock();
		jobs.emplace_back(new_job);
		mutex.unlock();
	}

	std::mutex mutex;
	std::vector<Job> jobs;
};
#pragma once

#include "Genesis/Job/Job.hpp"
#include "Genesis/Core/Types.hpp"

#include <concurrentqueue.h>
#include <thread>
#include <memory>

namespace Genesis
{
	class JobSystem
	{
	public:
		JobSystem();
		~JobSystem();

		void addJob(Job* job);
		void addJobAndWait(Job* job);

		void addJobs(vector<Job*> jobs);
		void addJobsAndWait(vector<Job*> jobs);

		inline bool shouldThreadsRun() { return this->should_threads_run; };
		inline uint8_t getThreadWaitTimeMilliseconds() { return this->thread_wait_time_milli; };

		moodycamel::ConcurrentQueue<Job*> job_queue;

	protected:
		vector<std::thread> threads;
		bool should_threads_run = true;
		uint8_t thread_wait_time_milli = 1;
	};
};
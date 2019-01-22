#pragma once

#include "Genesis/Job/Job.hpp"
#include "Genesis/Core/Types.hpp"
#include "Genesis/Job/JobQueue.hpp"

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
		Job* getJob();

		inline bool shouldThreadsRun() { return this->should_threads_run; };
		inline uint8_t getThreadWaitTimeMilliseconds() { return this->thread_wait_time_milli; };

	private:
		vector<std::thread> threads;
		bool should_threads_run = true;
		uint8_t thread_wait_time_milli = 1;

		JobQueue<Job*> job_queue;
	};
};
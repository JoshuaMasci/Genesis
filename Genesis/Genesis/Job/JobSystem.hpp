#pragma once

#include "Genesis/Core/Types.hpp"

#include <concurrentqueue.h>
#include <thread>
#include <memory>
#include <functional>

namespace Genesis
{
	typedef std::function<void(uint32_t)> JobType;

	struct JobStruct
	{
		JobType job;
		std::atomic<bool>* finished_flag = nullptr;
	};

	class JobSystem
	{
	public:
		JobSystem();
		~JobSystem();

		void addJob(JobType job);
		void addJobAndWait(JobType job);

		void addJobs(JobType* jobs, size_t job_count);
		void addJobsAndWait(JobType* jobs, size_t job_count);

		inline uint32_t getNumberOfJobThreads() { return (uint32_t)this->threads.size(); };

		//Just for Job Threads
		moodycamel::ConcurrentQueue<JobStruct> job_queue;
		inline bool shouldThreadsRun() { return this->should_threads_run; };

	protected:
		vector<std::thread> threads;
		bool should_threads_run = true;
	};
};
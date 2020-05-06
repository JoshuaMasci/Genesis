#pragma once

#include <concurrentqueue.h>
#include <thread>
#include <functional>

namespace Genesis
{
	typedef std::function<void(uint32_t)> JobType;
	typedef std::atomic<uint16_t> JobCounter;

	struct JobStruct
	{
		JobType job;
		JobCounter* job_counter = nullptr;
	};

	class JobSystem
	{
	public:
		JobSystem();
		~JobSystem();

		void addJob(JobType job, JobCounter* counter = nullptr);
		void addJobs(JobType* jobs, size_t job_count, JobCounter* counter = nullptr);

		inline uint32_t getNumberOfJobThreads() { return (uint32_t)this->threads.size(); };

		static void waitForCounter(JobCounter& counter);

		//Just for Job Threads
		moodycamel::ConcurrentQueue<JobStruct> job_queue;
		inline bool shouldThreadsRun() { return this->should_threads_run; };

	protected:
		vector<std::thread> threads;
		bool should_threads_run = true;
	};
};
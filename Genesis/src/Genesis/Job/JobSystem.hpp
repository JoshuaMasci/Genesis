#pragma once

#include "Genesis/Job/Job.hpp"
#include "Genesis/Core/Types.hpp"

#include <concurrentqueue.h>
#include <thread>
#include <memory>
#include <functional>

namespace Genesis
{
	class LambdaJob : public Job
	{
	public:
		LambdaJob() {};
		LambdaJob(std::function<void(uint32_t)> function);
		virtual void run(uint32_t thread_id) override;
	private:
		std::function<void(uint32_t)> function;
	};

	class JobSystem
	{
	public:
		JobSystem();
		~JobSystem();

		void addJob(Job* job);
		void addJobAndWait(Job* job);

		void addJobs(Job* jobs, size_t job_count);
		void addJobsAndWait(Job* jobs, size_t job_count);

		inline bool shouldThreadsRun() { return this->should_threads_run; };
		inline uint8_t getThreadWaitTimeMilliseconds() { return this->thread_wait_time_milli; };

		inline uint32_t getNumberOfJobThreads() { return (uint32_t)this->threads.size(); };

		moodycamel::ConcurrentQueue<Job*> job_queue;

	protected:
		vector<std::thread> threads;
		bool should_threads_run = true;
		uint8_t thread_wait_time_milli = 1;
	};
};
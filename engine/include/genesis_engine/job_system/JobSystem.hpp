#pragma once

#include <thread>

namespace Genesis
{
	typedef std::atomic<uint16_t> JobCounter;
	typedef void(*JobType)(void*);

	struct Job
	{
		void(*job)(void* data) = nullptr;
		void* data = nullptr;
		JobCounter* counter = nullptr;
	};

	class JobSystem
	{
	public:
		JobSystem();
		JobSystem(size_t thread_count);
		~JobSystem();

		void add_job(Job job);
		void add_jobs(Job* jobs, size_t job_count);

		inline uint32_t get_thread_count() { return (uint32_t)this->threads.size(); };
		static void wait_for_counter(JobCounter& counter);

	protected:
		friend void worker_thread(uint32_t thread_id, JobSystem* job_system);

		vector<std::thread> threads;
		bool threads_running = true;
		moodycamel::ConcurrentQueue<Job> job_queue;
	};
};
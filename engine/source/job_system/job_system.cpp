#include "genesis_engine/job_system/job_system.hpp"

#define sleep_time_milliseconds 2

namespace genesis
{
	void worker_thread(uint32_t thread_id, JobSystem* job_system)
	{
		GENESIS_ENGINE_INFO("Thread {} Start", thread_id);

		while (job_system->threads_running)
		{
			Job next_job;
			bool got_job = job_system->job_queue.try_dequeue(next_job);
			if (got_job)
			{
				next_job.job(next_job.data);

				if (next_job.counter != nullptr)
				{
					(*next_job.counter)--;
				}
			}
			else
			{
				std::this_thread::yield();
			}
		}

		GENESIS_ENGINE_INFO("Thread {} Exit", thread_id);
	}

	JobSystem::JobSystem()
	{
		uint32_t thread_count = std::thread::hardware_concurrency();
		this->threads.resize(thread_count);
		for (uint32_t i = 0; i < thread_count; i++)
		{
			this->threads[i] = std::thread(&worker_thread, i, this);
		}
	}

	JobSystem::JobSystem(size_t thread_count)
	{
		this->threads.resize(thread_count);
		for (uint32_t i = 0; i < thread_count; i++)
		{
			this->threads[i] = std::thread(&worker_thread, i, this);
		}
	}

	JobSystem::~JobSystem()
	{
		threads_running = false;

		for (uint32_t i = 0; i < this->threads.size(); i++)
		{
			this->threads[i].join();
		}
	}

	void genesis::JobSystem::add_job(Job job)
	{
		if (job.counter)
		{
			(*job.counter)++;
		}
		bool has_enqueued = this->job_queue.try_enqueue(job);
		GENESIS_ENGINE_ASSERT(has_enqueued == true, "Failed to enqueue job");
	}

	void JobSystem::add_jobs(Job* jobs, size_t job_count)
	{
		for (size_t i = 0; i < job_count; i++)
		{
			if (jobs[i].counter)
			{
				(*jobs[i].counter)++;
			}
			bool has_enqueued = this->job_queue.try_enqueue(jobs[i]);
			GENESIS_ENGINE_ASSERT(has_enqueued == true, "Failed to enqueue job");
		}
	}

	void JobSystem::wait_for_counter(JobCounter& counter)
	{
		while (counter != 0)
		{

		}
	}
}
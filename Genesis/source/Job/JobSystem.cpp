#include "Genesis/Job/JobSystem.hpp"

#include "Genesis/Debug/Log.hpp"
#include "Genesis/Debug/Assert.hpp"

#define sleep_time_milliseconds 2

using namespace Genesis;

void workerthread(uint32_t thread_id, JobSystem* job_system)
{
	GENESIS_ENGINE_INFO("Thread {} Start", thread_id);

	while (job_system->shouldThreadsRun())
	{
		JobStruct next_job;
		bool got_job = job_system->job_queue.try_dequeue(next_job);
		if (got_job)
		{
			next_job.job(thread_id);

			if (next_job.job_counter != nullptr)
			{
				(*next_job.job_counter)--;
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
	assert(thread_count >= 4);//Need 4 or more cores for the engine

	this->threads.resize(thread_count);
	for (uint32_t i = 0; i < thread_count; i++)
	{
		this->threads[i] = std::thread(&workerthread, i, this);
	}
}

JobSystem::~JobSystem()
{
	should_threads_run = false;

	for (uint32_t i = 0; i < this->threads.size(); i++)
	{
		//this->job_queue.enqueue(nullptr);//Push empty jobs to trigger exiting
	}

	for (uint32_t i = 0; i < this->threads.size(); i++)
	{
		this->threads[i].join();
	}
}

void Genesis::JobSystem::addJob(JobType job, JobCounter* counter)
{
	(*counter)++;

	bool has_enqueued = this->job_queue.try_enqueue({ job, counter });
	GENESIS_ENGINE_ASSERT_ERROR((has_enqueued == true), "Failed to enqueue job");
}

void JobSystem::addJobs(JobType* jobs, size_t job_count, JobCounter* counter)
{
	(*counter) += (uint16_t)job_count;

	for (size_t i = 0; i < job_count; i++)
	{
		bool has_enqueued = this->job_queue.try_enqueue({ jobs[i], counter });
		GENESIS_ENGINE_ASSERT_ERROR((has_enqueued == true), "Failed to enqueue job");
	}
}

void JobSystem::waitForCounter(JobCounter& counter)
{
	while (counter != 0)
	{
		
	}
}

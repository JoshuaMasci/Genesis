#include "JobSystem.hpp"

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

			if (next_job.finished_flag != nullptr)
			{
				(*next_job.finished_flag) = true;
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

void JobSystem::addJob(JobType job)
{
	bool has_enqueued = this->job_queue.try_enqueue({ job, nullptr });
	GENESIS_ENGINE_ASSERT_ERROR((has_enqueued == true), "Failed to enqueue job");
}

void JobSystem::addJobAndWait(JobType job)
{
	std::atomic<bool> finished_flag = false;

	bool has_enqueued = this->job_queue.try_enqueue({ job, &finished_flag });
	GENESIS_ENGINE_ASSERT_ERROR((has_enqueued == true), "Failed to enqueue job");

	while (!finished_flag)
	{
		std::this_thread::yield();
	}
}

void JobSystem::addJobs(JobType* jobs, size_t job_count)
{
	for (size_t i = 0; i < job_count; i++)
	{
		bool has_enqueued = this->job_queue.try_enqueue({ jobs[i], nullptr });
		GENESIS_ENGINE_ASSERT_ERROR((has_enqueued == true), "Failed to enqueue job");
	}
}

void JobSystem::addJobsAndWait(JobType* jobs, size_t job_count)
{
	vector<std::atomic<bool>> finished_flags(job_count);

	for (size_t i = 0; i < job_count; i++)
	{
		bool has_enqueued = this->job_queue.try_enqueue({ jobs[i], &finished_flags[i] });
		GENESIS_ENGINE_ASSERT_ERROR((has_enqueued == true), "Failed to enqueue job");
	}

	for (size_t i = 0; i < job_count; i++)
	{
		while (!finished_flags[i])
		{
			std::this_thread::yield();
		}
	}
}
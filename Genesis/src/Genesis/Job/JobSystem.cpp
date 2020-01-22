#include "JobSystem.hpp"

#include "Genesis/Debug/Log.hpp"

#define sleep_time_milliseconds 2

using namespace Genesis;

void workerthread(uint32_t thread_id, JobSystem* job_system)
{
	GENESIS_ENGINE_INFO("Thread {} Start", thread_id);

	while (job_system->shouldThreadsRun())
	{
		Job* job;
		bool got_job = job_system->job_queue.try_dequeue(job);
		if (got_job && job != nullptr)
		{
			job->run(thread_id);
			job->finish(); //Returns the value to indicate the job is done
		}
		else
		{
			//Sleep for a bit
			std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_milliseconds));
		}
	}

	GENESIS_ENGINE_INFO("Thread {} Exit", thread_id);
}

LambdaJob::LambdaJob(std::function<void(uint32_t)> function)
{
	this->function = function;
}

void LambdaJob::run(uint32_t thread_id)
{
	this->function(thread_id);
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
		this->job_queue.enqueue(nullptr);//Push empty jobs to trigger exiting
	}

	for (uint32_t i = 0; i < this->threads.size(); i++)
	{
		this->threads[i].join();
	}
}

void JobSystem::addJob(Job* job)
{
	if (job != nullptr)
	{
		this->job_queue.enqueue(job);
	}
}

void JobSystem::addJobAndWait(Job* job)
{
	this->addJob(job);
	job->waitTillFinished();
}

void JobSystem::addJobs(Job* jobs, size_t job_count)
{
	for (size_t i = 0; i < job_count; i++)
	{
		this->job_queue.enqueue(jobs + i);
	}
}

void JobSystem::addJobsAndWait(Job* jobs, size_t job_count)
{
	for (size_t i = 0; i < job_count; i++)
	{
		this->job_queue.enqueue(jobs + i);
	}

	for (size_t i = 0; i < job_count; i++)
	{
		jobs[i].waitTillFinished();
	}
}
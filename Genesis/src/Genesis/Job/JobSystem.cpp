#include "JobSystem.hpp"

#define sleep_time_milliseconds 2

using namespace Genesis;

void workerthread(uint32_t thread_id, JobSystem* job_system)
{
	printf("Thread %d Start\n", thread_id);

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

	printf("Thread %d Exit\n", thread_id);
}

JobSystem::JobSystem()
{
	uint32_t cores = std::thread::hardware_concurrency();
	assert(cores >= 4);//Need 4 or more cores for the engine

	for (uint32_t i = 0; i < cores; i++)
	{
		this->threads.push_back(std::thread(&workerthread, i, this));
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

void JobSystem::addJobs(vector<Job*> jobs)
{
	this->job_queue.enqueue_bulk(jobs.begin(), jobs.size());
}

void JobSystem::addJobsAndWait(vector<Job*> jobs)
{
	this->job_queue.enqueue_bulk(jobs.begin(), jobs.size());

	for (Job* job : jobs)
	{
		job->waitTillFinished();
	}
}

void JobSystem::addJobsAndWait(Array<Job>& jobs)
{
	for (size_t i = 0; i < jobs.size(); i++)
	{
		this->job_queue.enqueue(&jobs[i]);
	}

	for (size_t i = 0; i < jobs.size(); i++)
	{
		jobs[i].waitTillFinished();
	}
}

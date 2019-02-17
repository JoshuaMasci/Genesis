#include "JobSystem.hpp"

void workerthread(unsigned int thread_id, Genesis::JobSystem* job_system)
{
	printf("Thread %d Start\n", thread_id);

	while (job_system->shouldThreadsRun())
	{
		Genesis::Job* job = job_system->job_queue.pop();
		if (job != nullptr)
		{
			job->run();
			job->finish(); //Returns the value to indicate the job is done
		}
	}

	printf("Thread %d Exit\n", thread_id);
}

Genesis::JobSystem::JobSystem()
{
	unsigned int cores = std::thread::hardware_concurrency();
	assert(cores >= 4);//Need 4 or more cores for the engine
	cores -= 2;//Leave 2 threads for OS and other tasks

	for (unsigned int i = 0; i < cores; i++)
	{
		this->threads.push_back(std::thread(&workerthread, i, this));
	}
}

Genesis::JobSystem::~JobSystem()
{
	should_threads_run = false;

	for (int i = 0; i < this->threads.size(); i++)
	{
		this->job_queue.push(nullptr);//Push empty jobs to trigger exiting
	}

	for (int i = 0; i < this->threads.size(); i++)
	{
		this->threads[i].join();
	}
}

void Genesis::JobSystem::addJob(Job* job)
{
	if (job != nullptr)
	{
		this->job_queue.push(job);
	}
}

void Genesis::JobSystem::addJobAndWait(Job* job)
{
	this->addJob(job);
	job->waitTillFinished();
}

void Genesis::JobSystem::addJobs(vector<Job*> jobs)
{
	for (Job* job : jobs)
	{
		this->addJob(job);
	}
}

void Genesis::JobSystem::addJobsAndWait(vector<Job*> jobs)
{
	for (Job* job : jobs)
	{
		this->addJob(job);
	}

	for (Job* job : jobs)
	{
		job->waitTillFinished();
	}
}

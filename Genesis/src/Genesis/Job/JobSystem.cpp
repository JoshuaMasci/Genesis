#include "JobSystem.hpp"

void workerthread(unsigned int thread_id, Genesis::JobSystem* job_system)
{
	while (job_system->shouldThreadsRun())
	{
		Genesis::Job* job = job_system->getJob();
		if (job != nullptr)
		{
			job->run();
			job->finish();
			delete job;
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(job_system->getThreadWaitTimeMilliseconds()));
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
		this->threads[i].join();
	}
}

void Genesis::JobSystem::addJob(Job* job)
{
	this->job_queue.push(job);
}

Genesis::Job* Genesis::JobSystem::getJob()
{
	return this->job_queue.pop();
}

#include "Application.hpp"

using namespace Genesis;

Application::Application()
	:input_manager("config/input")
{
	for (int i = 0; i < 12345; i++)
	{
		const auto entity = this->entity_registry.create();
		this->entity_registry.assign<Test>(entity);
	}
}

Application::~Application() 
{
	if (this->window != nullptr)
	{
		delete this->window;
	}

	if (this->platform != nullptr)
	{
		delete this->platform;
	}
}

void Application::runFrame(double delta_time)
{
	this->input_manager.update();

	if (this->platform != nullptr)
	{
		this->platform->onUpdate(delta_time);
	}

	/*entt::view<unsigned int, Test> view = this->entity_registry.view<Test>();
	size_t num_of_entities = view.size();
	if (num_of_entities > 0)
	{
		size_t entities_per_job = 1000;
		size_t num_of_jobs = num_of_entities / entities_per_job;

		vector<Job*> jobs;

		if (num_of_jobs == 0)
		{
			jobs = vector<Job*>(1);
			jobs[0] = new TestJob(&view, &this->entity_registry, 0, num_of_entities - 1);
		}
		else
		{
			jobs = vector<Job*>(num_of_jobs);
			size_t entity_index = 0;
			size_t number_left = num_of_entities % entities_per_job;
			size_t left_per_job = 
			for (size_t i = 0; i < jobs.size(); i++)
			{
				size_t end_index = entity_index + (entities_per_job - 1);

				if (number_left > 0)
				{
					end_index++;
					number_left--;
				}

				jobs[i] = new TestJob(&view, &this->entity_registry, entity_index, end_index);
				entity_index = end_index + 1;
			}
		}

		this->job_system.addJobsAndWait(jobs);

		for (size_t i = 0; i < jobs.size(); i++)
		{
			delete jobs[i];
		}
	}*/

	if (this->window != nullptr)
	{
		this->window->updateBuffer();
	}
}

void Application::close()
{
	this->is_running = false;
	//TODO emit event
}

bool Application::isRunning()
{ 
	return this->is_running; 
}

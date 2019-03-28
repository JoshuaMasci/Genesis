#include "GameScene.hpp"

#include "Genesis/Application.hpp"

#include "Genesis/Physics/PhysicsUpdateJob.hpp"

using namespace Genesis;

GameScene::GameScene(Application* app)
{
	this->application = app;

}

GameScene::~GameScene()
{

}

void GameScene::runFrame(double delta_time)
{
	//Physics Update
	if (!this->physics_worlds.empty())
	{
		vector<Job*> physics_jobs(this->physics_worlds.size());
		int i = 0;
		for (auto world : this->physics_worlds)
		{
			physics_jobs[i] = new PhysicsUpdateJob(world.second, delta_time);
			i++;
		}

		this->application->job_system.addJobsAndWait(physics_jobs);

		for (int i = 0; i < physics_jobs.size(); i++)
		{
			delete physics_jobs[i];
		}
	}
}
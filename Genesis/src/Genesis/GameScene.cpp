#include "GameScene.hpp"

#include "Genesis/Application.hpp"

#include "Genesis/WorldTransform.hpp"
#include "Genesis/Physics/RigidBody.hpp"

using namespace Genesis;

GameScene::GameScene(Application* app)
{
	this->application = app;

	this->render_system = new RenderSystem(this->application->window);

	this->temp = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(this->temp);
	this->entity_registry.assign<RigidBody>(this->temp);
}

GameScene::~GameScene()
{
	delete this->render_system;
}

void GameScene::runSimulation(double delta_time)
{
	this->physics_system.runSimulation(this->entity_registry, this->application->job_system, delta_time);
}

void GameScene::drawFrame(double delta_time)
{
	this->render_system->drawFrame(this->entity_registry, this->application->job_system, delta_time);
}

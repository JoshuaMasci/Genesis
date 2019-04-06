#include "GameScene.hpp"

#include "Genesis/Application.hpp"

#include "Genesis/WorldTransform.hpp"
#include "Genesis/Physics/RigidBody.hpp"

using namespace Genesis;

GameScene::GameScene(Application* app)
{
	this->application = app;

	this->temp = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(this->temp);
	this->entity_registry.assign<RigidBody>(this->temp);
}

GameScene::~GameScene()
{

}

void GameScene::runSimulation(double delta_time)
{
	this->physics_system.runSimulation(this->entity_registry, this->application->job_system, delta_time);

	//WorldTransform& transform = this->entity_registry.get<WorldTransform>(this->temp);
	//printf("Y pos: %lf\n", transform.current_transform.getPosition().y);
}

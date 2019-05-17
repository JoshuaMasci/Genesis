#include "GameScene.hpp"

#include "Genesis/Application.hpp"

#include "Genesis/WorldTransform.hpp"
#include "Genesis/Physics/RigidBody.hpp"

#include "Genesis/Rendering/OpenGL/Camera.hpp"

using namespace Genesis;

GameScene::GameScene(Application* app)
{
	this->application = app;

	this->temp = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(this->temp);
	//this->entity_registry.assign<RigidBody>(this->temp);

	this->camera = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(this->camera, vector3D(0.0, 0.0, -5.0));

	this->renderer = new Renderer(this->application->rendering_backend);
}

GameScene::~GameScene()
{
	if (this->renderer != nullptr)
	{
		delete this->renderer;
	}
}

void GameScene::runSimulation(double delta_time)
{
	this->physics_system.runSimulation(this->entity_registry, this->application->job_system, delta_time);
}

void GameScene::drawFrame(double delta_time)
{
	this->renderer->drawFrame();
}

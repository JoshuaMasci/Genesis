#include "GameScene.hpp"

#include "Genesis/Application.hpp"

#include "Genesis/WorldTransform.hpp"
#include "Genesis/Physics/RigidBody.hpp"

#include "Genesis/Rendering/OpenGL/Camera.hpp"

#include "Genesis/Rendering/Renderer.hpp"

using namespace Genesis;

GameScene::GameScene(Application* app)
{
	this->application = app;

	this->temp = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(this->temp);
	this->entity_registry.assign<Model>(this->temp, "resources/meshes/cube.obj", "");

	this->camera = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(this->camera, vector3D(0.0, 0.0, -5.0));

	this->renderer = new Renderer(this->application->rendering_backend);
	this->renderer->loadMesh("resources/meshes/cube.obj");

	for (int i = 0; i < 5; i++)
	{
		EntityId entity = this->entity_registry.create();
		this->entity_registry.assign<WorldTransform>(entity).current_transform.setPosition(vector3D(i * 1.1, 0.0, 0.0));
		this->entity_registry.assign<Model>(entity, "resources/meshes/cube.obj", "");
	}
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
	this->renderer->drawFrame(this->entity_registry);
}

#include "GameScene.hpp"

#include "Genesis/Application.hpp"

#include "Genesis/WorldTransform.hpp"
#include "Genesis/Physics/RigidBody.hpp"

#include "Genesis/Rendering/Renderer.hpp"

#include "Genesis/Rendering/Camera.hpp"

#include "Genesis/DebugCamera.hpp"

using namespace Genesis;

GameScene::GameScene(Application* app)
{
	this->application = app;

	this->temp = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(this->temp, vector3D(0.0, 0.0, 0.0), glm::angleAxis(3.1415926/2.0, vector3D(0.0, 1.0, 0.0)));
	this->entity_registry.assign<Model>(this->temp, "resources/meshes/cube1.obj", "resources/textures/1k_Grid.png");

	this->camera = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(this->camera, vector3D(0.0, 0.75, -2.5));
	this->entity_registry.assign<Camera>(this->camera, 75.0f);
	this->entity_registry.assign<DebugCamera>(this->camera, 0.5, 0.2);

	this->renderer = new Renderer(this->application->rendering_backend);
	this->renderer->loadMesh("resources/meshes/cube1.obj");
	this->renderer->loadTexture("resources/textures/1k_Grid.png");
	/*this->renderer->loadTexture("resources/textures/4k_Grid.png");
	this->renderer->loadTexture("resources/textures/Red.png");
	this->renderer->loadTexture("resources/textures/Blue.png");
	this->renderer->loadTexture("resources/textures/Green.png");*/
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
	DebugCamera::update(&this->application->input_manager, &this->entity_registry.get<DebugCamera>(this->camera), &this->entity_registry.get<WorldTransform>(this->camera), delta_time);
}

void GameScene::drawFrame(double delta_time)
{
	this->renderer->drawFrame(this->entity_registry, this->camera);
}

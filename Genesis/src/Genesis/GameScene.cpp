#include "GameScene.hpp"

#include "Genesis/Application.hpp"

#include "Genesis/WorldTransform.hpp"

#include "Genesis/Rendering/Renderer.hpp"

#include "Genesis/Rendering/Camera.hpp"

#include "Genesis/DebugCamera.hpp"

using namespace Genesis;

GameScene::GameScene(Application* app)
{
	this->application = app;
	this->renderer = new Renderer(this->application->rendering_backend);
	this->ui_renderer = new ImGuiRenderer(this->application->rendering_backend);

	this->temp = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(this->temp, vector3D(0.0, 0.0, 0.0), glm::angleAxis(3.1415926/2.0, vector3D(0.0, 1.0, 0.0)));
	this->entity_registry.assign<Model>(this->temp);
	this->entity_registry.get<Model>(this->temp) = this->renderer->createModel("resources/meshes/cube.obj", "resources/textures/1k_grid.png", "resources/shaders/Vulkan/texture");

	this->camera = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(this->camera, vector3D(0.0, 0.75, -5));
	this->entity_registry.assign<Camera>(this->camera, 75.0f);
	this->entity_registry.assign<DebugCamera>(this->camera, 0.5, 0.2);
}

GameScene::~GameScene()
{
	if (this->renderer != nullptr)
	{
		this->renderer->destroyModel(this->entity_registry.get<Model>(this->temp));
		delete this->renderer;
	}

	if (this->ui_renderer != nullptr)
	{
		delete this->ui_renderer;
	}
}

void GameScene::runSimulation(double delta_time)
{
	//this->physics_system.runSimulation(this->entity_registry, this->application->job_system, delta_time);
	DebugCamera::update(&this->application->input_manager, &this->entity_registry.get<DebugCamera>(this->camera), &this->entity_registry.get<WorldTransform>(this->camera), delta_time);
}

void GameScene::drawFrame(double delta_time)
{
	this->renderer->drawFrame(this->entity_registry, this->camera);

	this->ui_renderer->startFrame();
	this->ui_renderer->endFrame();

	this->renderer->endFrame();
}

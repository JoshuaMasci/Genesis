#include "GameScene.hpp"

#include "Genesis/Application.hpp"
#include "Genesis/WorldTransform.hpp"
#include "Genesis/Rendering/Renderer.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/DebugCamera.hpp"
#include "Genesis/Rendering/ResourceLoaders.hpp"

#include <fstream>

using namespace Genesis;


GameScene::GameScene(Application* app)
{
	this->application = app;

	this->camera = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(this->camera);
	this->entity_registry.assign<Camera>(this->camera, 90.0f);
	this->entity_registry.assign<DebugCamera>(this->camera, 0.5, 0.2);
	this->entity_registry.assign<SpotLight>(this->camera, 22.0f, 300.0f, vector2F(0.16f, 0.0f), vector3F(0.2f, 1.0f, 0.1f), 0.2f, false);


	VertexInputDescription description({ {"", VertexElementType::float_1 } });
	vector<float> temp_stuff(100);
	VertexBuffer vertex_buffer = this->application->rendering_backend->createVertexBuffer(temp_stuff.data(), temp_stuff.size() * sizeof(float), description, MemoryUsage::GPU_Only);
}

GameScene::~GameScene()
{

	if (this->renderer != nullptr)
	{
		delete this->renderer;
	}

	if (this->ui_renderer != nullptr)
	{
		delete this->ui_renderer;
	}

	this->application->rendering_backend->destroyShader(this->screen_shader);
}

void GameScene::runSimulation(double delta_time)
{
	//this->physics_system.runSimulation(this->entity_registry, this->application->job_system, delta_time);
	DebugCamera::update(&this->application->input_manager, &this->entity_registry.get<DebugCamera>(this->camera), &this->entity_registry.get<WorldTransform>(this->camera), delta_time);
}

#include "imgui.h"

void GameScene::drawWorld(double delta_time)
{
	if (this->application->rendering_backend->beginFrame() == true)
	{

		this->application->rendering_backend->endFrame();
	}
}

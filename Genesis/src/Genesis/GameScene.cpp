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

	Shader shader = ShaderLoader::loadShaderSingle(this->application->rendering_backend, "imgui/vulkan");
	this->application->rendering_backend->destroyShader(shader);

	this->application->rendering_backend->destroyTexture(PngLoader::loadTexture(this->application->rendering_backend, "test.png"));

	this->uniform_buffers.resize(100);
	for (size_t i = 0; i < this->uniform_buffers.size(); i++)
	{
		this->uniform_buffers[i] = this->application->rendering_backend->createUniformBuffer(64, MemoryUsage::GPU_Only);
	}

	/*FramebufferLayout layout({}, ImageFormat::D_16_Unorm);
	View view = this->application->rendering_backend->createView(layout, vector2U(1024));
	this->application->rendering_backend->beginView(view);
	this->application->rendering_backend->endView(view);
	this->application->rendering_backend->destroyView(view);*/
}

GameScene::~GameScene()
{
	for (size_t i = 0; i < this->uniform_buffers.size(); i++)
	{
		this->application->rendering_backend->destroyUniformBuffer(this->uniform_buffers[i]);
	}

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
	for (size_t i = 0; i < this->uniform_buffers.size(); i++)
	{
		List<uint8_t> empty_list(64);
		this->application->rendering_backend->setUniform(this->uniform_buffers[i], empty_list.data(), 64);
	}

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

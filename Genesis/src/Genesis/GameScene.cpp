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

	this->screen_shader = ShaderLoader::loadShaderSingle(this->application->rendering_backend, "res/temp_shaders/test");

	struct Vertex 
	{
		glm::vec2 pos;
		glm::vec3 color;
	};
	const vector<Vertex> vertices = 
	{
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	const vector<uint16_t> indices =
	{
		0, 1, 2,
	};

	VertexInputDescription vertex_description
	({
		{"in_position", VertexElementType::float_2},
		{"in_color", VertexElementType::float_3},
	});

	this->vertex = this->application->rendering_backend->createVertexBuffer((void*)vertices.data(), vertices.size() * sizeof(Vertex), vertex_description);
	this->index = this->application->rendering_backend->createIndexBuffer((void*)indices.data(), indices.size() * sizeof(uint16_t), IndexType::uint16);
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

	this->application->rendering_backend->destroyVertexBuffer(this->vertex);
	this->application->rendering_backend->destroyIndexBuffer(this->index);

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
	CommandBuffer* command_buffer = this->application->rendering_backend->beginFrame();
	if (command_buffer != nullptr)
	{
		PipelineSettings settings;
		settings.cull_mode = CullMode::None;
		settings.depth_test = DepthTest::None;
		
		command_buffer->setPipelineSettings(settings);
		command_buffer->setShader(this->screen_shader);

		vector3F color(0.0f, 0.0f, 1.0f);
		command_buffer->setUniformConstant(&color, sizeof(vector3F));
		command_buffer->setVertexBuffer(this->vertex, VertexInputDescription());
		command_buffer->setIndexBuffer(this->index, IndexType::uint16);
		command_buffer->drawIndexed(3, 0, 1, 0);
		this->application->rendering_backend->endFrame();
	}
}

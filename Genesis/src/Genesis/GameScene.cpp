#include "GameScene.hpp"

#include "Genesis/Application.hpp"

#include "Genesis/WorldTransform.hpp"

#include "Genesis/Rendering/Renderer.hpp"

#include "Genesis/Rendering/Camera.hpp"

#include "Genesis/DebugCamera.hpp"

#include <fstream>

using namespace Genesis;

GameScene::GameScene(Application* app)
{
	this->application = app;
	this->renderer = new Renderer(this->application->rendering_backend);
	this->ui_renderer = new ImGuiRenderer(this->application->rendering_backend, &this->application->input_manager);

	{
		string vert_file_path = "resources/shaders/vulkan/whole_screen.vert.spv";
		string frag_file_path = "resources/shaders/vulkan/whole_screen.frag.spv";

		string vert_data = "";
		string frag_data = "";

		std::ifstream vert_file(vert_file_path, std::ios::ate | std::ios::binary);
		if (vert_file.is_open())
		{
			size_t fileSize = (size_t)vert_file.tellg();
			vert_file.seekg(0);
			vert_data.resize(fileSize);
			vert_file.read(vert_data.data(), vert_data.size());
			vert_file.close();
		}

		std::ifstream frag_file(frag_file_path, std::ios::ate | std::ios::binary);
		if (frag_file.is_open())
		{
			size_t fileSize = (size_t)frag_file.tellg();
			frag_file.seekg(0);
			frag_data.resize(fileSize);
			frag_file.read(frag_data.data(), frag_data.size());
			frag_file.close();
		}

		this->screen_shader = this->application->rendering_backend->createShader(vert_data, frag_data);
	}

	this->temp = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(this->temp, vector3D(0.0, 0.0, 0.0), glm::angleAxis(3.1415926/2.0, vector3D(0.0, 1.0, 0.0)));
	this->entity_registry.assign<Model>(this->temp);
	this->entity_registry.get<Model>(this->temp) = this->renderer->createModel("resources/meshes/cube.obj", "resources/textures/1k_grid.png", "resources/shaders/Vulkan/texture");

	for (int i = 1; i <= 100; i++)
	{
		EntityId cuber = this->entity_registry.create();
		this->entity_registry.assign<WorldTransform>(cuber, vector3D(0.0, 0.0, 2.0 * i));
		this->entity_registry.assign<Model>(cuber);
		this->entity_registry.get<Model>(cuber) = this->renderer->createModel("resources/meshes/cube.obj", "resources/textures/Red.png", "resources/shaders/Vulkan/texture");
	}

	this->camera = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(this->camera, vector3D(0.0, 0.75, -5));
	this->entity_registry.assign<Camera>(this->camera, 75.0f);
	this->entity_registry.assign<DebugCamera>(this->camera, 0.5, 0.2);
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

void GameScene::drawFrame(double delta_time)
{
	bool result = this->application->rendering_backend->beginFrame();

	if (result == true)
	{
		vector<ViewHandle> sub_views;

		this->renderer->drawFrame(this->entity_registry, this->camera);

		this->ui_renderer->startFrame();
		this->ui_renderer->endFrame();

		CommandBuffer* screen_buffer = this->application->rendering_backend->getScreenCommandBuffer();

		PipelineSettings settings;
		settings.depth_test = DepthTest::None;
		settings.blend_op = BlendOp::Add;
		settings.src_factor = BlendFactor::Alpha_Src;
		settings.dst_factor = BlendFactor::One_Minus_Alpha_Src;

		screen_buffer->setShader(this->screen_shader);
		screen_buffer->setPipelineSettings(settings);
		VertexBufferHandle screen_vertex = this->application->rendering_backend->getWholeScreenQuadVertex();
		IndexBufferHandle screen_index = this->application->rendering_backend->getWholeScreenQuadIndex();

		screen_buffer->setUniformView("in_texture", this->renderer->view, 0);
		screen_buffer->drawIndexed(screen_vertex, screen_index);
		sub_views.push_back(this->renderer->view);

		screen_buffer->setUniformView("in_texture", this->ui_renderer->getView(), this->ui_renderer->getViewImageIndex());
		screen_buffer->drawIndexed(screen_vertex, screen_index);
		sub_views.push_back(this->ui_renderer->getView());
		
		this->application->rendering_backend->endFrame();
		this->application->rendering_backend->submitFrame(sub_views);
	}
}

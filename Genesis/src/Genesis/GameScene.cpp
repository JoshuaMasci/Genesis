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
	this->renderer = new Renderer(this->application->rendering_backend);
	this->ui_renderer = new ImGuiRenderer(this->application->rendering_backend, &this->application->input_manager);

	this->screen_shader = ShaderLoader::loadShaderSingle(this->application->rendering_backend, "resources/shaders/vulkan/whole_screen");

	this->temp = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(this->temp, vector3D(0.0, 0.0, 0.0), glm::angleAxis(3.1415926/2.0, vector3D(0.0, 1.0, 0.0)));
	this->entity_registry.assign<Model>(this->temp, "resources/meshes/cube.obj", "resources/textures/1k_grid.png", "resources/shaders/vulkan/texture");

	EntityId rock = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(rock, vector3D(0.0, 0.0, 2.0));
	this->entity_registry.assign<Model>(rock, "resources/test_rock/rock.obj", "resources/test_rock/rock_d.png", "resources/shaders/vulkan/texture");

	EntityId plane = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(plane, vector3D(0.0, -3.0, 0.0));
	this->entity_registry.assign<Model>(plane, "resources/meshes/plane.obj", "resources/textures/4k_grid.png", "resources/shaders/vulkan/texture");

	this->camera = this->entity_registry.create();
	this->entity_registry.assign<WorldTransform>(this->camera, vector3D(0.0, 0.75, -2.0));
	this->entity_registry.assign<Camera>(this->camera, 75.0f);
	this->entity_registry.assign<DebugCamera>(this->camera, 0.5, 0.2);

	this->directional_light = this->entity_registry.create();
	this->entity_registry.assign<DirectionalLight>(this->directional_light, vector3F(1.0f), 0.1F);
	this->entity_registry.get<DirectionalLight>(this->directional_light).casts_shadows = true;
	this->entity_registry.get<DirectionalLight>(this->directional_light).shadow_size = vector2F(10.0f);
	this->entity_registry.assign<WorldTransform>(this->directional_light, vector3D(0.0, 10.0, 0.0), glm::angleAxis(3.1415926 / 2.0, vector3D(1.0, 0.0, 0.0)));
	this->entity_registry.assign<SpotLight>(this->directional_light, 0.5f, 30.0f, vector3F(0.0f, 0.1f, 0.01f), vector3F(0.4f), 1.0f);

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
	bool result = this->application->rendering_backend->beginFrame();

	if (result == true)
	{
		vector<View> sub_views;

		this->renderer->startFrame();
		this->renderer->drawWorld(this->entity_registry, this->camera);
		this->renderer->endFrame();

		this->directional = this->entity_registry.get<DirectionalLight>(this->directional_light);
		this->spot = this->entity_registry.get<SpotLight>(this->directional_light);
		this->ui_renderer->startFrame();
		{
			ImGui::Begin("Directional Light");
			ImGui::SliderFloat("Intensity", &this->directional.intensity, 0.0f, 10.0f);
			ImGui::ColorEdit3("Color", &this->directional.color.x);
			ImGui::ColorEdit3("Ambient Light", &this->renderer->ambient_light.x);
			ImGui::End();

			ImGui::Begin("Spot Light");
			ImGui::SliderFloat("Range", &this->spot.range, 0.0f, 100.0f);
			ImGui::SliderFloat("Cutoff", &this->spot.cutoff, 0.0f, 1.0f);
			ImGui::SliderFloat("Intensity", &this->spot.intensity, 0.0f, 10.0f);
			ImGui::ColorEdit3("Color", &this->spot.color.x);
			ImGui::SliderFloat3("Attenuation", &this->spot.attenuation.x, 0.0f, 1.0f);
			ImGui::Checkbox("Cast_Shadows", &this->spot.casts_shadows);

			ImGui::End();

		}

		this->ui_renderer->endFrame();
		this->entity_registry.get<DirectionalLight>(this->directional_light) = this->directional;
		this->entity_registry.get<SpotLight>(this->directional_light) = this->spot;

		CommandBuffer* screen_buffer = this->application->rendering_backend->getScreenCommandBuffer();

		PipelineSettings settings;
		settings.depth_test = DepthTest::None;
		settings.blend_op = BlendOp::Add;
		settings.src_factor = BlendFactor::Alpha_Src;
		settings.dst_factor = BlendFactor::One_Minus_Alpha_Src;

		screen_buffer->setShader(this->screen_shader);
		screen_buffer->setPipelineSettings(settings);
		VertexBuffer screen_vertex = this->application->rendering_backend->getWholeScreenQuadVertex();
		IndexBuffer screen_index = this->application->rendering_backend->getWholeScreenQuadIndex();

		screen_buffer->setUniformView("in_texture", this->renderer->getView(), this->renderer->getViewImageIndex());
		screen_buffer->drawIndexed(screen_vertex, screen_index);
		sub_views.push_back(this->renderer->getView());

		screen_buffer->setUniformView("in_texture", this->ui_renderer->getView(), this->ui_renderer->getViewImageIndex());
		screen_buffer->drawIndexed(screen_vertex, screen_index);
		sub_views.push_back(this->ui_renderer->getView());
		
		this->application->rendering_backend->endFrame();
		this->application->rendering_backend->submitFrame(sub_views);
	}
}

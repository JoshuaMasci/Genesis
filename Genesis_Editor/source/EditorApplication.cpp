#include "Genesis_Editor/EditorApplication.hpp"

//Hardcoded for now
#include "SDL2_Platform.hpp" 
#include "SDL2_Window.hpp" 
#include "OpenglBackend.hpp"
#include "Genesis/LegacyRendering/LegacyImGui.hpp"
#include "Genesis/Platform/FileSystem.hpp"

#include "imgui.h"

//Components
#include "Genesis/Component/ModelComponent.hpp"
#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"
#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/CollisionShape.hpp"
#include "Genesis/Physics/ReactPhyscis.hpp"

#include "Genesis/Resource/ObjLoader.hpp"

#include "Genesis/World/Entity.hpp"
#include "Genesis/World/SceneSystem.hpp"
#include "Genesis/World/PhysicsSystem.hpp"

#include <jsoncons/json.hpp>
#include <fstream>
using namespace jsoncons;

namespace Genesis
{
	void loadSceneTemp(World* world, const string& json_file, MeshPool* mesh_pool, MaterialPool* material_pool);

	EditorApplication::EditorApplication()
	{
		this->console_window = new ConsoleWindow();
		Logging::console_sink->setConsoleWindow(this->console_window);

		this->platform = new SDL2_Platform(this);
		this->window = new SDL2_Window(vector2U(1600, 900), "Genesis Editor");

		this->legacy_backend = new Opengl::OpenglBackend((SDL2_Window*)window);
		this->ui_renderer = new LegacyImGui(this->legacy_backend, this->input_manager, this->window);

		this->mesh_pool = new MeshPool(this->legacy_backend);
		this->texture_pool = new TexturePool(this->legacy_backend);
		this->material_pool = new MaterialPool(this->texture_pool);

		this->entity_hierarchy_window = new EntityHierarchyWindow();
		this->entity_properties_window = new EntityPropertiesWindow(this->mesh_pool, this->material_pool);
		this->scene_window = new SceneWindow(this->input_manager, this->legacy_backend);
		this->asset_browser_window = new AssetBrowserWindow(this->legacy_backend);
		this->material_editor_window = new MaterialEditorWindow(this->material_pool, this->texture_pool);
		this->material_editor_window->setActiveMaterial(this->material_pool->getResource("res/materials/grid.mat"));

		{
			this->world_simulator.addWorldSystem(SceneSystem());
			this->world_simulator.addWorldSystem(PhysicsSystem());
			this->editor_world.components.add<SceneInfo>()->ambient_light = vector3F(0.1f);

			Entity* entity = new Entity(0, "Camera_Entity");
			entity->local_transform.setPosition(vector3D(0.0, 0.0, -3.0));
			entity->components.add<Camera>();
			entity->components.add<DirectionalLight>(vector3F(1.0f), 1.0f, true);
			World::addEntity(&this->editor_world, entity);

			Entity* sphere = new Entity(0, "Sphere");
			sphere->components.add<ModelComponent>(this->mesh_pool->getResource("res/meshes/sphere.obj"), this->material_pool->getResource("res/materials/red.mat"));
			World::addEntity(&this->editor_world, sphere);

			loadSceneTemp(&this->editor_world, "res/ground.entity", this->mesh_pool, this->material_pool);
		}
	}

	EditorApplication::~EditorApplication()
	{
		delete this->mesh_pool;
		delete this->texture_pool;
		delete this->material_pool;

		delete this->console_window;
		delete this->entity_hierarchy_window;
		delete this->entity_properties_window;
		delete this->scene_window;
		delete this->asset_browser_window;
		delete this->material_editor_window;

		delete this->legacy_backend;

		Logging::console_sink->setConsoleWindow(nullptr);
	}

	void EditorApplication::update(TimeStep time_step)
	{
		GENESIS_PROFILE_FUNCTION("EditorApplication::update");
		Application::update(time_step);

		this->scene_window->update(time_step);

		if (this->scene_window->isSceneRunning()) {}

		this->world_simulator.simulateWorld(time_step, &this->editor_world);
	}

	void EditorApplication::render(TimeStep time_step)
	{
		GENESIS_PROFILE_FUNCTION("EditorApplication::render");

		Application::render(time_step);

		this->legacy_backend->startFrame();
		vector4F clear_color = vector4F(0.0f, 0.0f, 0.0f, 1.0f);
		float clear_depth = 1.0f;
		this->legacy_backend->clearFramebuffer(true, true, &clear_color, &clear_depth);

		this->ui_renderer->beginFrame();
		this->ui_renderer->beginDocking();
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load Scene", ""))
				{

				}

				if (ImGui::MenuItem("Exit", "")) { this->close(); };
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				ImGui::MenuItem("Imgui Demo", nullptr, &this->show_demo_window);
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		this->ui_renderer->endDocking();

		{
			FrameStats stats = this->legacy_backend->getLastFrameStats();
			ImGui::Begin("Stats");
			ImGui::Text("Frame Time (ms): %.2f", time_step * 1000.0);
			ImGui::Text("Draw Calls     : %u", stats.draw_calls);
			ImGui::Text("Tris count     : %u", stats.triangles_count);
			ImGui::End();
		}

		this->console_window->draw();
		this->entity_hierarchy_window->draw(&this->editor_world, this->mesh_pool, this->material_pool);
		this->entity_properties_window->draw(this->entity_hierarchy_window->getSelected());
		this->scene_window->draw(this->editor_world.components.get<SceneInfo>());
		this->asset_browser_window->draw("res/");
		this->material_editor_window->draw();

		if (this->show_demo_window)
		{
			ImGui::ShowDemoWindow();
		}

		this->ui_renderer->endFrame();

		this->legacy_backend->endFrame();
	}

	Entity* loadEntity(string name, json& json_entity, MeshPool* mesh_pool, MaterialPool* material_pool)
	{
		Entity* entity = new Entity(0, name.c_str());

		if (json_entity.contains("Transform"))
		{
			json& json_transform = json_entity["Transform"];
			TransformD& transform = entity->local_transform;

			vector<double> position = json_transform["position"].as<vector<double>>();
			transform.setPosition(vector3D(position[0], position[1], position[2]));

			vector<double> orientation = json_transform["orientation"].as<vector<double>>();
			transform.setOrientation(quaternionD(orientation[3], orientation[0], orientation[1], orientation[2]));

			vector<double> scale = json_transform["scale"].as<vector<double>>();
			transform.setScale(vector3D(scale[0], scale[1], scale[2]));
		}

		if (json_entity.contains("Model"))
		{
			json& json_model = json_entity["Model"];
			ModelComponent* model = entity->components.add<ModelComponent>();
			model->mesh = mesh_pool->getResource(json_model["mesh"].as_string());
			model->material = material_pool->getResource(json_model["material"].as_string());
		}

		if (json_entity.contains("RigidBody"))
		{
			json& json_rigid_body = json_entity["RigidBody"];
			RigidBody* rigid_body = entity->components.add<RigidBody>();
			rigid_body->setType((RigidBodyType)json_rigid_body["type"].as<int>());
			rigid_body->setMass(json_rigid_body["mass"].as_double());
			rigid_body->setGravityEnabled(json_rigid_body["gravity_enabled"].as_bool());
			rigid_body->setIsAllowedToSleep(json_rigid_body["is_allowed_to_sleep"].as_bool());

			vector<double> linear_velocity = json_rigid_body["linear_velocity"].as<vector<double>>();
			rigid_body->setLinearVelocity(vector3D(linear_velocity[0], linear_velocity[1], linear_velocity[2]));

			vector<double> angular_velocity = json_rigid_body["angular_velocity"].as<vector<double>>();
			rigid_body->setAngularVelocity(vector3D(angular_velocity[0], angular_velocity[1], angular_velocity[2]));
		}

		return entity;
	}

	void loadSceneTemp(World* world, const string& json_file, MeshPool* mesh_pool, MaterialPool* material_pool)
	{
		std::ifstream in_stream(json_file);

		if (!in_stream.is_open())
		{
			GENESIS_ENGINE_ASSERT("Failed to open scene file {}", json_file);
			return;
		}

		jsoncons::json scene_file = json::parse(in_stream);

		for (const auto& json_entity : scene_file.object_range())
		{
			Entity* entity = loadEntity(json_entity.key(), (jsoncons::json)json_entity.value(), mesh_pool, material_pool);
			World::addEntity(world, entity);
		}
	}
}
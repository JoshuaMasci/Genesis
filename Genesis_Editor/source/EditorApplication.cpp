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
#include "Genesis/Component/TransformComponent.hpp"

//World Components
#include "Genesis/Physics/PhysicsWorld.hpp"

//Systems
#include "Genesis/System/EntitySystemSet.hpp"
#include "Genesis/System/TransformResolveSystem.hpp"
#include "Genesis/System/SceneSystem.hpp"
#include "Genesis/Scene/SceneSerializer.hpp"

#include "Genesis/Scene/ScenePrototype.hpp"

namespace Genesis
{
	EditorApplication::EditorApplication()
	{
		this->console_window = new ConsoleWindow();
		Logging::console_sink->setConsoleWindow(this->console_window);

		this->platform = new SDL2_Platform(this);
		this->window = new SDL2_Window(vector2U(1600, 900), "Genesis Editor");

		this->legacy_backend = new Opengl::OpenglBackend((SDL2_Window*)window);
		this->ui_renderer = new LegacyImGui(this->legacy_backend, this->input_manager, this->window);

		this->resource_manager = new ResourceManager(this->legacy_backend);

		this->entity_hierarchy_window = new EntityHierarchyWindow(this->resource_manager);
		this->entity_properties_window = new EntityPropertiesWindow(this->resource_manager);
		this->scene_window = new SceneWindow(this->input_manager, this->legacy_backend);
		this->asset_browser_window = new AssetBrowserWindow(this->legacy_backend, "res/");
		this->material_editor_window = new MaterialEditorWindow(this->resource_manager);
		this->material_editor_window->setActiveMaterial(this->resource_manager->material_pool.getResource("res/materials/grid.mat"));

		this->runtime_scene = new Scene();
		this->runtime_scene->scene_components.add<SceneInfo>();

		this->editor_scene = new ScenePrototype();
	}

	EditorApplication::~EditorApplication()
	{
		delete this->runtime_scene;
		delete this->editor_scene;

		delete this->resource_manager;

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

		TransformResolveSystem().run(this->runtime_scene, time_step);

		if (this->scene_window->is_scene_running()) 
		{
			if (this->runtime_scene->scene_components.has<PhysicsWorld>())
			{
				auto& pre_view = this->runtime_scene->registry.view<RigidBody, Transform>();
				for (EntityHandle entity : pre_view)
				{
					pre_view.get<RigidBody>(entity).setTransform(pre_view.get<Transform>(entity));
				}

				PhysicsWorld& physics = this->runtime_scene->scene_components.get<PhysicsWorld>();
				physics.simulate(time_step);

				auto& post_view = this->runtime_scene->registry.view<RigidBody, Transform>();
				for (EntityHandle entity : post_view)
				{
					post_view.get<RigidBody>(entity).getTransform(post_view.get<Transform>(entity));
				}
			}
		}
		else
		{

		}

		this->scene_window->update(time_step);
	}


	void build_scene_info_entity(SceneInfo& scene_info, EntityPrototype* entity, const TransformD& parent_world_transform)
	{
		TransformD world_transform = TransformUtils::transformBy(parent_world_transform, entity->get_transform());

		ModelComponent* model_component = entity->get_component<ModelComponent>();
		if (model_component)
		{
			scene_info.models.push_back({ model_component->mesh , model_component->material, world_transform });
		}

		DirectionalLight* directional_light = entity->get_component<DirectionalLight>();
		if (directional_light)
		{
			scene_info.directional_lights.push_back({ *directional_light, world_transform });
		}

		PointLight* point_light = entity->get_component<PointLight>();
		if (point_light)
		{
			scene_info.point_lights.push_back({ *point_light, world_transform });
		}

		for (auto child : entity->get_children())
		{
			build_scene_info_entity(scene_info, child, world_transform);
		}
	}

	void build_scene_info(SceneInfo& scene_info, ScenePrototype* prototype)
	{
		scene_info.clear_buffers();

		TransformD transform;
		for (auto entity : prototype->get_entities())
		{
			build_scene_info_entity(scene_info, entity, transform);
		}
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
				if (ImGui::MenuItem("Open Scene Prototype", ""))
				{
					string save_file_path = FileSystem::openFileDialog("Supported Files(*.scene)\0*.scene;\0All files(*.*)\0*.*\0");

					if (!save_file_path.empty())
					{
						delete this->editor_scene;
						this->editor_scene = SceneSerializer().deserialize_prototype(save_file_path.c_str(), this->resource_manager);
					}
				}

				if (ImGui::MenuItem("Save Scene Prototype", ""))
				{
					string save_file_path = FileSystem::saveFileDialog("Supported Files(*.scene)\0*.scene;\0All files(*.*)\0*.*\0");

					if (!save_file_path.empty())
					{
						SceneSerializer().serialize_prototype(this->editor_scene, save_file_path.c_str());
					}
				}

				if (ImGui::MenuItem("Open Scene", ""))
				{
					string save_file_path = FileSystem::openFileDialog("Supported Files(*.scene)\0*.scene;\0All files(*.*)\0*.*\0");

					if (!save_file_path.empty())
					{
						delete this->runtime_scene;
						this->runtime_scene = SceneSerializer().deserialize(save_file_path.c_str(), this->resource_manager);

						if (!this->runtime_scene->scene_components.has<SceneInfo>())
						{
							this->runtime_scene->scene_components.add<SceneInfo>();
						}

						if (!this->runtime_scene->scene_components.has<PhysicsWorld>())
						{
							this->runtime_scene->scene_components.add<PhysicsWorld>(vector3D(0.0, -9.8, 0.0));
						}
					}
				}

				if (ImGui::MenuItem("Save Scene", ""))
				{
					string save_file_path = FileSystem::saveFileDialog("Supported Files(*.scene)\0*.scene;\0All files(*.*)\0*.*\0");

					if (!save_file_path.empty())
					{
						SceneSerializer().serialize(this->runtime_scene, save_file_path.c_str());
					}
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
		this->entity_hierarchy_window->draw(this->editor_scene);
		this->entity_properties_window->draw(this->entity_hierarchy_window->get_selected());

		SceneSystem::build_scene(this->runtime_scene);
		
		build_scene_info(this->scene_info, this->editor_scene);

		this->scene_window->draw(this->scene_info);
		this->asset_browser_window->draw();
		this->material_editor_window->draw();

		if (this->show_demo_window)
		{
			ImGui::ShowDemoWindow();
		}

		this->ui_renderer->endFrame();

		this->legacy_backend->endFrame();
	}
}
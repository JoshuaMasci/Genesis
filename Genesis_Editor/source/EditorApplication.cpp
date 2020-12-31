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

//Game
#include "Genesis/Game/Chunk.hpp"
#include "Genesis/Game/ChunkMeshGenerator.hpp"

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

		this->editor_scene = new Scene();
		this->editor_scene->scene_components.add<SceneInfo>();

		ChunkMeshGenerator mesh_generator(this->legacy_backend);

		for (size_t x = 0; x < 16; x++)
		{
			for (size_t z = 0; z < 16; z++)
			{
				Entity chunk = this->editor_scene->createEntity("Chunk");
				//chunk.add<Transform>(vector3D(32.0 * x, -32.0, 32.0 * z));
				chunk.add<WorldTransform>().setTransform(vector3D(32.0 * x, -32.0, 32.0 * z));
				chunk.add<DefaultChunk>();
				chunk.add<ModelComponent>().mesh = shared_ptr<Mesh>(mesh_generator.generateMesh(chunk.get<DefaultChunk>()));
				chunk.get<ModelComponent>().material = this->resource_manager->material_pool.getResource("res/materials/red.mat");
			}
		}
	}

	EditorApplication::~EditorApplication()
	{
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

		TransformResolveSystem().run(this->editor_scene, time_step);

		if (this->scene_window->is_scene_running()) 
		{
			if (this->editor_scene->scene_components.has<PhysicsWorld>())
			{
				auto& pre_view = this->editor_scene->registry.view<RigidBody, Transform>();
				for (EntityHandle entity : pre_view)
				{
					pre_view.get<RigidBody>(entity).setTransform(pre_view.get<Transform>(entity));
				}

				PhysicsWorld& physics = this->editor_scene->scene_components.get<PhysicsWorld>();
				physics.simulate(time_step);

				auto& post_view = this->editor_scene->registry.view<RigidBody, Transform>();
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
				if (ImGui::MenuItem("Open Scene", ""))
				{
					string save_file_path = FileSystem::openFileDialog("Supported Files(*.scene)\0*.scene;\0All files(*.*)\0*.*\0");

					if (!save_file_path.empty())
					{
						delete this->editor_scene;
						this->editor_scene = SceneSerializer().deserialize(save_file_path.c_str(), this->resource_manager);

						if (!this->editor_scene->scene_components.has<SceneInfo>())
						{
							this->editor_scene->scene_components.add<SceneInfo>();
						}

						if (!this->editor_scene->scene_components.has<PhysicsWorld>())
						{
							this->editor_scene->scene_components.add<PhysicsWorld>(vector3D(0.0, -9.8, 0.0));
						}
					}
				}

				if (ImGui::MenuItem("Save Scene", ""))
				{
					string save_file_path = FileSystem::saveFileDialog("Supported Files(*.scene)\0*.scene;\0All files(*.*)\0*.*\0");

					if (!save_file_path.empty())
					{
						SceneSerializer().serialize(this->editor_scene, save_file_path.c_str());
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
		this->entity_properties_window->draw(this->entity_hierarchy_window->getSelected());

		SceneSystem::build_scene(this->editor_scene);

		this->scene_window->draw(this->editor_scene);
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
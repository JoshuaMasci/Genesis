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

#include "Genesis/System/EntitySystemSet.hpp"
#include "Genesis/System/TransformResolveSystem.hpp"
#include "Genesis/System/SceneSystem.hpp"

#include "Genesis/Scene/SceneSerializer.hpp"

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

		this->mesh_pool = new MeshPool(this->legacy_backend);
		this->texture_pool = new TexturePool(this->legacy_backend);
		this->material_pool = new MaterialPool(this->texture_pool);

		this->entity_hierarchy_window = new EntityHierarchyWindow();
		this->entity_properties_window = new EntityPropertiesWindow();
		this->scene_window = new SceneWindow(this->input_manager, this->legacy_backend);
		this->asset_browser_window = new AssetBrowserWindow(this->legacy_backend, "res/");
		this->material_editor_window = new MaterialEditorWindow(this->material_pool, this->texture_pool);
		this->material_editor_window->setActiveMaterial(this->material_pool->getResource("res/materials/grid.mat"));

		this->editor_scene = new Scene();
		this->editor_scene->scene_components.add<SceneInfo>();
	}

	EditorApplication::~EditorApplication()
	{
		delete this->editor_scene;

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

		TransformResolveSystem().run(this->editor_scene, time_step);

		if (this->scene_window->is_scene_running()) 
		{

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
						this->editor_scene = SceneSerializer().deserialize(save_file_path.c_str(), this->mesh_pool, this->material_pool);
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
		this->entity_hierarchy_window->draw(this->editor_scene, this->mesh_pool, this->material_pool);
		this->entity_properties_window->draw(this->entity_hierarchy_window->getSelected(), this->mesh_pool, this->material_pool);

		SceneSystem::build_scene(this->editor_scene);

		this->scene_window->draw(&this->editor_scene->scene_components.get<SceneInfo>());
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
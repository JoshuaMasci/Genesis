#include "Genesis_Editor/EditorApplication.hpp"

int main(int argc, char** argv)
{
	GENESIS_PROFILE_START(); 
	Genesis::Logging::inti_engine_logging();
	Genesis::Logging::inti_client_logging("Genesis_Editor");

	Genesis::EditorApplication* editor = new Genesis::EditorApplication();
	GENESIS_INFO("Genesis_Editor Started");

	GENESIS_PROFILE_BLOCK_START("Genesis_Editor Loop");
	editor->run();
	GENESIS_PROFILE_BLOCK_END();

	GENESIS_PROFILE_BLOCK_START("Genesis_Editor Exit");
	delete editor;
	GENESIS_PROFILE_BLOCK_END();

	GENESIS_INFO("Genesis_Editor Closed");

	GENESIS_PROFILE_WRITE_TO_FILE("Genesis_profile.prof");

	//Wait till enter
	//getchar();

	return 0;
}

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
#include "Genesis/Component/Hierarchy.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"
#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/CollisionShape.hpp"
#include "Genesis/Physics/ReactPhyscis.hpp"

#include "Genesis/Resource/ObjLoader.hpp"

namespace Genesis
{
	EditorApplication::EditorApplication()
	{
		this->console_window = new ConsoleWindow();
		Logging::console_sink->setConsoleWindow(this->console_window);

		this->platform = new SDL2_Platform(this);
		this->window = new SDL2_Window(vector2U(1600, 900), "Genesis Editor");

		this->legacy_backend = new Opengl::OpenglBackend((SDL2_Window*) window);
		this->ui_renderer = new LegacyImGui(this->legacy_backend, this->input_manager, this->window);

		this->mesh_pool = new MeshPool(this->legacy_backend);
		this->texture_pool = new TexturePool(this->legacy_backend);
		this->material_pool = new MaterialPool(this->texture_pool);

		this->temp_material = this->material_pool->getResource("res/materials/grid.mat");

		this->entity_hierarchy_window = new EntityHierarchyWindow();
		this->entity_properties_window = new EntityPropertiesWindow(this->mesh_pool, this->material_pool);
		this->scene_window = new SceneWindow(this->input_manager, this->legacy_backend);
		this->asset_browser_window = new AssetBrowserWindow(this->legacy_backend);
		this->material_editor_window = new MaterialEditorWindow(this->material_pool, this->texture_pool);
		this->material_editor_window->setActiveMaterial(this->temp_material);

		this->editor_world = new EntityWorld();


		{
			Entity entity = this->editor_world->createEntity("Test_Entity");
			entity.addComponent<TransformD>().setPosition(vector3D(0.0, 0.0, -3.0));
			entity.addComponent<Camera>();
			entity.addComponent<DirectionalLight>(vector3F(1.0f), 1.0f, true);
			//entity.addComponent<PointLight>(20.0f, vector2F(1.0f), vector3F(1.0f), 0.4f, true);
		}

		{
			Entity entity = this->editor_world->createEntity("Physics Object");
			entity.addComponent<TransformD>();
			entity.addComponent<ModelComponent>(this->mesh_pool->getResource("res/meshes/sphere.obj"), this->material_pool->getResource("res/materials/red.mat"));
			entity.addComponent<CollisionShape>();
		}
	}

	EditorApplication::~EditorApplication()
	{
		this->temp_material.reset();

		delete this->editor_world;

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

		if (this->scene_window->isSceneRunning())
		{
			this->editor_world->runSimulation(time_step);
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
				if (ImGui::MenuItem("Load Scene", ""))
				{
					string filename = FileSystem::getFileDialog("res/");

					GENESIS_ENGINE_INFO("Opening {}", filename);

					Entity entity = this->editor_world->createEntity("Mesh Test");
					entity.addComponent<TransformD>().setOrientation(glm::angleAxis(glm::radians(25.0), vector3D(0.0, 0.0, 1.0)));
					//entity.addComponent<ModelComponent>(this->mesh_pool->getResource(filename), &this->temp_material);

				}

				if (ImGui::MenuItem("Exit", "")) { this->close(); };
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		this->ui_renderer->endDocking();

		//ImGui::ShowDemoWindow();

		{
			FrameStats stats = this->legacy_backend->getLastFrameStats();
			ImGui::Begin("Stats");
			ImGui::LabelText(std::to_string(time_step * 1000.0).c_str(), "Frame Time (ms)");
			ImGui::LabelText(std::to_string(stats.draw_calls).c_str(), "Draw Calls");
			ImGui::LabelText(std::to_string(stats.triangles_count).c_str(), "Tris count");
			ImGui::End();
		}

		this->console_window->draw();
		this->entity_hierarchy_window->draw(*this->editor_world->getRegistry());
		this->entity_properties_window->draw(*this->editor_world->getRegistry(), this->entity_hierarchy_window->getSelected());
		this->scene_window->draw(*this->editor_world);
		this->asset_browser_window->draw("res/");
		this->material_editor_window->draw();

		this->ui_renderer->endFrame();

		this->legacy_backend->endFrame();
	}
}
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
#include "Genesis/Component/MeshComponent.hpp"
#include "Genesis/Component/TransformComponents.hpp"
#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Component/Hierarchy.hpp"
#include "Genesis/Resource/PbrMesh.hpp"
#include "Genesis/Resource/PbrMaterial.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"
#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/ReactPhyscis.hpp"

#include "Genesis/Resource/ObjLoader.hpp"

namespace Genesis
{
	EditorApplication::EditorApplication()
	{
		this->platform = new SDL2_Platform(this);
		this->window = new SDL2_Window(vector2U(1600, 900), "Genesis Editor");

		this->legacy_backend = new Opengl::OpenglBackend((SDL2_Window*) window);
		this->ui_renderer = new LegacyImGui(this->legacy_backend, this->input_manager, this->window);

		this->console_window = new ConsoleWindow();
		Logging::console_sink->setConsoleWindow(this->console_window);

		this->entity_hierarchy_window = new EntityHierarchyWindow();
		this->entity_properties_window = new EntityPropertiesWindow();
		this->scene_window = new SceneWindow(this->input_manager, this->legacy_backend);

		this->mesh_pool = new MeshPool(this->legacy_backend);

		this->temp_material.albedo_factor = vector4F(1.0f, 0.0f, 1.0, 1.0f);

		EntityRegistry* editor_registry = this->editor_world.getRegistry();

		{
			EntityHandle entity = editor_registry->create();
			editor_registry->assign<NameComponent>(entity, "Test_Entity");
			editor_registry->assign<TransformD>(entity).setOrientation(glm::angleAxis(glm::radians(80.0), vector3D(1.0f, 0.0, 0.1)));
			editor_registry->assign<WorldTransform>(entity) = editor_registry->get<TransformD>(entity);
			editor_registry->assign<Camera>(entity);
			editor_registry->assign<DirectionalLight>(entity, vector3F(1.0f), 0.4f, true);
		}

		{
			EntityHandle entity = editor_registry->create();
			editor_registry->assign<NameComponent>(entity, "Physics Cube");
			editor_registry->assign<TransformD>(entity);
			editor_registry->assign<WorldTransform>(entity) = editor_registry->get<TransformD>(entity);
			editor_registry->assign<MeshComponent>(entity, this->mesh_pool->getResource("res/cube.obj"), &this->temp_material);
			TransformD transform = editor_registry->get<TransformD>(entity);
			editor_registry->assign<RigidBody>(entity);
		}
	}

	EditorApplication::~EditorApplication()
	{
		delete this->mesh_pool;

		delete this->console_window;
		delete this->entity_hierarchy_window;
		delete this->entity_properties_window;
		delete this->scene_window;

		delete this->legacy_backend;

		Logging::console_sink->setConsoleWindow(nullptr);
	}

	void EditorApplication::update(TimeStep time_step)
	{
		GENESIS_PROFILE_FUNCTION("EditorApplication::update");
		Application::update(time_step);

		this->scene_window->udpate(time_step);

		if (this->scene_window->isSceneRunning())
		{
			this->editor_world.runSimulation(time_step);
		}

		this->editor_world.resolveTransforms();
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

					EntityRegistry* editor_registry = this->editor_world.getRegistry();
					EntityHandle entity = editor_registry->create();
					editor_registry->assign<NameComponent>(entity, "Mesh Test");
					editor_registry->assign<TransformD>(entity).setOrientation(glm::angleAxis(glm::radians(25.0), vector3D(0.0, 0.0, 1.0)));
					editor_registry->assign<WorldTransform>(entity) = editor_registry->get<TransformD>(entity);
					editor_registry->assign<MeshComponent>(entity, this->mesh_pool->getResource(filename), &this->temp_material);
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

		this->console_window->drawWindow();
		this->entity_hierarchy_window->drawWindow(*this->editor_world.getRegistry());
		this->entity_properties_window->drawWindow(*this->editor_world.getRegistry(), this->entity_hierarchy_window->getSelected());
		this->scene_window->drawWindow(this->editor_world);

		this->ui_renderer->endFrame();

		this->legacy_backend->endFrame();
	}
}
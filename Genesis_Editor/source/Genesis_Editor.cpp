#include "Genesis_Editor/EditorApplication.hpp"

int main(int argc, char** argv)
{
	GENESIS_PROFILE_START(); 
	Genesis::Logging::inti_engine_logging("");
	Genesis::Logging::inti_client_logging("Genesis_Editor", "");

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

//Hardcode For now
#include "SDL2_Platform.hpp" 
#include "SDL2_Window.hpp" 
#include "OpenglBackend.hpp"
#include "Genesis/LegacyRendering/LegacyImGui.hpp"

#include "Genesis/Rendering/Camera.hpp"

#include "imgui.h"

//Components
#include "Genesis/Component/NameComponent.hpp"

namespace Genesis
{
	EditorApplication::EditorApplication()
	{
		this->platform = new SDL2_Platform(this);
		this->window = new SDL2_Window(vector2U(1600, 900), "Genesis Editor");

		this->legacy_backend = new Opengl::OpenglBackend((SDL2_Window*) window);
		this->ui_renderer = new LegacyImGui(this->legacy_backend, this->input_manager, this->window);


		this->console_window = new ConsoleWindow();
		//Logging::console_sink->setConsoleWindow(this->console_window);

		this->world_view_window = new WorldViewWindow();
		this->entity_properties_window = new EntityPropertiesWindow();
		this->scene_view_window = new SceneViewWindow(this->legacy_backend);

		this->editor_registry = new EntityRegistry();
		this->editor_registry->registerComponent<TransformD>();
		this->editor_registry->registerComponent<NameComponent>();

		this->editor_base_world = this->editor_registry->createWorld();

		EntityId entity = this->editor_base_world->createEntity<TransformD, NameComponent>();
		NameComponent* name = this->editor_base_world->getComponent<NameComponent>(entity);
		strcpy_s(name->data, "Test_Entity");
		this->editor_base_world->initalizeComponent<TransformD>(entity);
	}

	EditorApplication::~EditorApplication()
	{
		delete this->editor_registry;

		delete this->console_window;
		delete this->world_view_window;
		delete this->entity_properties_window;
		delete this->scene_view_window;

		delete this->legacy_backend;

		//Logging::console_sink->setConsoleWindow(nullptr);
	}

	void EditorApplication::update(TimeStep time_step)
	{
		GENESIS_PROFILE_FUNCTION("EditorApplication::update");
		Application::update(time_step);
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
				if (ImGui::MenuItem("Exit", "")) { this->close(); };
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		this->ui_renderer->endDocking();

		{
			FrameStats stats = this->legacy_backend->getLastFrameStats();
			ImGui::Begin("Stats");
			ImGui::LabelText(std::to_string(time_step * 1000.0).c_str(), "Frame Time (ms)");
			ImGui::LabelText(std::to_string(stats.draw_calls).c_str(), "Draw Calls");
			ImGui::LabelText(std::to_string(stats.triangles_count).c_str(), "Tris count");
			ImGui::End();
		}


		this->console_window->drawWindow();
		this->world_view_window->drawWindow(this->editor_registry, this->editor_base_world);
		this->entity_properties_window->drawWindow(this->editor_registry, this->editor_base_world, this->world_view_window->getSelected());
		this->scene_view_window->drawWindow(this->editor_base_world);

		this->ui_renderer->endFrame();

		this->legacy_backend->endFrame();
	}
}
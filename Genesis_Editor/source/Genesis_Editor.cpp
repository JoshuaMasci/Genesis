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
#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Component/Hierarchy.hpp"
#include "Genesis/Resource/PbrMesh.hpp"
#include "Genesis/Resource/PbrMaterial.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"
#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/CollisionShape.hpp"
#include "Genesis/Physics/ReactPhyscis.hpp"

#include "Genesis/Resource/ObjLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Genesis
{
	uint8_t* loadTexture(const string& texture_file_path, vector2I& size, int32_t& channels)
	{
		return stbi_load(texture_file_path.c_str(), (int*)(&size.x), (int*)(&size.y), (int*)(&channels), STBI_default);
	}

	void unloadTexture(uint8_t* data)
	{
		stbi_image_free(data);
	}

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

		{
			//Load image
			vector2I size;
			int32_t channels;
			uint8_t* data = loadTexture("res/Ground Alien 02_normal.png", size, channels);

			TextureCreateInfo create_info = {};
			create_info.size = size;

			switch (channels)
			{
			case 1:
				create_info.format = ImageFormat::R_8;
				break;
			case 2:
				create_info.format = ImageFormat::RG_8;
				break;
			case 3:
				create_info.format = ImageFormat::RGB_8;
				break;
			case 4:
				create_info.format = ImageFormat::RGBA_8;
				break;
			}

			this->temp_material.normal_texture.texture = this->legacy_backend->createTexture(create_info, data);
			this->temp_material.normal_texture.uv = 0;

			unloadTexture(data);
		}


		{
			//Load image
			vector2I size;
			int32_t channels;
			uint8_t* data = loadTexture("res/1k_grid.png", size, channels);

			TextureCreateInfo create_info = {};
			create_info.size = size;

			switch (channels)
			{
			case 1:
				create_info.format = ImageFormat::R_8;
				break;
			case 2:
				create_info.format = ImageFormat::RG_8;
				break;
			case 3:
				create_info.format = ImageFormat::RGB_8;
				break;
			case 4:
				create_info.format = ImageFormat::RGBA_8;
				break;
			}

			this->temp_material.albedo_texture.texture = this->legacy_backend->createTexture(create_info, data);
			this->temp_material.albedo_texture.uv = 0;

			unloadTexture(data);
		}

		{
			Entity entity = this->editor_world.createEntity("Test_Entity");
			//entity.addComponent<TransformD>().setOrientation(glm::angleAxis(glm::radians(80.0), vector3D(1.0f, 0.0, 0.1)));
			entity.addComponent<TransformD>().setPosition(vector3D(0.0, 0.0, -3.0));
			entity.addComponent<Camera>();
			//entity.addComponent<DirectionalLight>(vector3F(1.0f), 0.4f, true);
			entity.addComponent<PointLight>(20.0f, vector2F(1.0f), vector3F(1.0f), 0.4f, true);
		}

		{
			Entity entity = this->editor_world.createEntity("Physics Cube");
			entity.addComponent<TransformD>();
			entity.addComponent<MeshComponent>(this->mesh_pool->getResource("res/sphere.obj"), &this->temp_material);
			entity.addComponent<RigidBody>();
			entity.addComponent<CollisionShape>();
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

		{
			ImGui::Begin("Material");
			ImGui::ColorEdit4("albedo", &this->temp_material.albedo_factor.x);
			ImGui::SliderFloat2("metallic roughness", &this->temp_material.metallic_roughness_factor.x, 0.0f, 1.0f);
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
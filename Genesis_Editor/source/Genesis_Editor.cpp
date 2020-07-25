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

//Hardcoded for now
#include "SDL2_Platform.hpp" 
#include "SDL2_Window.hpp" 
#include "OpenglBackend.hpp"
#include "Genesis/LegacyRendering/LegacyImGui.hpp"

#include "imgui.h"

//Components
#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Resource/PbrMesh.hpp"
#include "Genesis/Resource/PbrMaterial.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"
#include "Genesis/Physics/RigidBody.hpp"

#include "Genesis/ECS/EntitySystem.hpp"

namespace Genesis
{
	void loadNode(EntityWorld* world, GltfNode* node)
	{
		if (node == nullptr)
		{
			return;
		}

		EntityId entity = world->createEntity();

		world->addComponent<TransformD>(entity, TransformUtils::toTransformD(node->local_transform));

		if (!node->name.empty())
		{
			world->addComponent<NameComponent>(entity, node->name.c_str());
		}

		if (node->mesh != nullptr)
		{
			(*world->addComponent<PbrMesh>(entity)) = *node->mesh;
			(*world->addComponent<PbrMaterial>(entity)) = *node->mesh->primitives[0].temp_material_ptr;

			world->addComponent<RigidBody>(entity);
		}

		for (auto child_node : node->child_nodes)
		{
			loadNode(world, child_node);
		}
	}

	void loadScene(EntityWorld* world, GltfModel* scene)
	{
		for (auto root_node : scene->root_nodes)
		{
			loadNode(world, root_node);
		}
	}

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
		this->scene_view_window = new SceneViewWindow(this->input_manager, this->legacy_backend);

		this->editor_registry = new EntityRegistry();
		this->editor_registry->registerComponent<TransformD>();
		this->editor_registry->registerComponent<NameComponent>();
		this->editor_registry->registerComponent<PbrMesh>();
		this->editor_registry->registerComponent<PbrMaterial>();
		this->editor_registry->registerComponent<Camera>();
		this->editor_registry->registerComponent<DirectionalLight>();
		this->editor_registry->registerComponent<PointLight>();
		this->editor_registry->registerComponent<SpotLight>();
		this->editor_registry->registerComponent<RigidBody>();

		{
			this->editor_base_world = this->editor_registry->createWorld();

			EntityId entity = this->editor_base_world->createEntity();
			this->editor_base_world->addComponent<NameComponent>(entity, "Test_Entity");
			this->editor_base_world->addComponent<TransformD>(entity)->setOrientation( glm::angleAxis(glm::radians(90.0), vector3D(1.0f, 0.0, 0.0)) );
			this->editor_base_world->addComponent<Camera>(entity);
			this->editor_base_world->addComponent<DirectionalLight>(entity, vector3F(1.0f), 0.4f, true);

			entt::entity entity2 = this->editor_registry_entt.create();
			this->editor_registry_entt.assign<NameComponent>(entity2, "Test_Entity");
			this->editor_registry_entt.assign<TransformD>(entity2).setOrientation(glm::angleAxis(glm::radians(90.0), vector3D(1.0f, 0.0, 0.0)));
			this->editor_registry_entt.assign<Camera>(entity2);
			this->editor_registry_entt.assign<DirectionalLight>(entity2, vector3F(1.0f), 0.4f, true);
		}

		{
			const string file_name = "res/Shapes.glb";

			tinygltf::Model gltfModel;
			tinygltf::TinyGLTF loader;
			string error;
			string warning;
			bool return_value;

			if (file_name.substr(file_name.find_last_of(".") + 1) == "gltf")
			{
				return_value = loader.LoadASCIIFromFile(&gltfModel, &error, &warning, file_name);
			}
			else if (file_name.substr(file_name.find_last_of(".") + 1) == "glb")
			{
				return_value = loader.LoadBinaryFromFile(&gltfModel, &error, &warning, file_name);
			}
			else
			{
				GENESIS_ENGINE_ERROR("Unknown File extension");
			}

			if (!error.empty())
			{
				GENESIS_ENGINE_ERROR("Error: {}", error);
			}

			if (!warning.empty())
			{
				GENESIS_ENGINE_WARNING("Warning: {}", warning);
			}

			if (!return_value)
			{
				GENESIS_ENGINE_CRITICAL("Failed to parse glTF");
			}

			this->scene_model = new GltfModel(this->legacy_backend, gltfModel);
			loadScene(this->editor_base_world, this->scene_model);
		}

		const ComponentId transform_id = this->editor_registry->getComponentID<TransformD>();
		const ComponentId rigidbody_id = this->editor_registry->getComponentID<RigidBody>();
		MultiComponentView view = this->editor_base_world->getMultiComponentView({ transform_id, rigidbody_id });
		for (size_t i = 0; i < view.getSize(); i++)
		{
			TransformD* transform = (TransformD*)view.getComponent(i, 0);
			RigidBody* rigidbody = (RigidBody*)view.getComponent(i, 1);
			if (transform != nullptr && rigidbody != nullptr)
			{
				GENESIS_ENGINE_INFO("Rigidbody");
			}
		}



		/*this->editor_base_world->forEachPool<RigidBody, TransformD>([&](EntityPool* pool)
		{
			for (size_t i = 0; i < pool->getEntityCount(); i++)
			{
				GENESIS_ENGINE_INFO("Rigidbody");
			}
		});*/
	}

	EditorApplication::~EditorApplication()
	{
		delete this->scene_model;

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

		this->scene_view_window->udpate(time_step);
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
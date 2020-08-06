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

#include "imgui.h"

//Components
#include "Genesis/Component/TransformComponents.hpp"
#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Component/Hierarchy.hpp"
#include "Genesis/Resource/PbrMesh.hpp"
#include "Genesis/Resource/PbrMaterial.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"
#include "Genesis/Physics/RigidBody.hpp"

namespace Genesis
{
	void loadNode(entt::registry& world, GltfNode* node)
	{
		if (node == nullptr)
		{
			return;
		}

		EntityHandle entity = world.create();

		world.assign<WorldTransform>(entity, TransformUtils::toTransformD(node->local_transform));

		if (!node->name.empty())
		{
			world.assign<NameComponent>(entity, node->name.c_str());
		}

		if (node->mesh != nullptr)
		{
			world.assign<PbrMesh>(entity, *node->mesh);
			world.assign<PbrMaterial>(entity, *node->mesh->primitives[0].temp_material_ptr);
			world.assign<RigidBody>(entity);
		}

		for (auto child_node : node->child_nodes)
		{
			loadNode(world, child_node);
		}
	}

	void loadScene(entt::registry& world, GltfModel* scene)
	{
		for (auto root_node : scene->root_nodes)
		{
			loadNode(world, root_node);
		}
	}

	EntityHandle createHierarchy(EntityRegistry& registry, size_t count, char* name_temp)
	{
		EntityHandle entity = registry.create();

		string name = name_temp + std::to_string(count);
		registry.assign<NameComponent>(entity, name.c_str());

		if (count > 1)
		{
			EntityHandle child = createHierarchy(registry, count - 1, name_temp);
			Hierarchy::addChild(registry, entity, child);
		}

		return entity;
	}

	EditorApplication::EditorApplication()
	{
		this->platform = new SDL2_Platform(this);
		this->window = new SDL2_Window(vector2U(1600, 900), "Genesis Editor");

		this->legacy_backend = new Opengl::OpenglBackend((SDL2_Window*) window);
		this->ui_renderer = new LegacyImGui(this->legacy_backend, this->input_manager, this->window);

		this->console_window = new ConsoleWindow();
		Logging::console_sink->setConsoleWindow(this->console_window);

		this->entity_list_window = new EntityListWindow();
		this->entity_properties_window = new EntityPropertiesWindow();
		this->scene_window = new SceneWindow(this->input_manager, this->legacy_backend);

		{
			EntityHandle entity = this->editor_registry.create();
			this->editor_registry.assign<NameComponent>(entity, "Test_Entity");
			this->editor_registry.assign<WorldTransform>(entity).setOrientation(glm::angleAxis(glm::radians(90.0), vector3D(1.0f, 0.0, 0.0)));
			this->editor_registry.assign<Camera>(entity);
			this->editor_registry.assign<DirectionalLight>(entity, vector3F(1.0f), 0.4f, true);
		}

		createHierarchy(editor_registry, 3, "Entity Hiearchy ");

		{
			const string file_name = "res/RoundShip.glb";

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
			loadScene(this->editor_registry, this->scene_model);
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

		delete this->console_window;
		delete this->entity_list_window;
		delete this->entity_properties_window;
		delete this->scene_window;

		delete this->legacy_backend;

		Logging::console_sink->setConsoleWindow(nullptr);
	}

	TransformD calcWorldTransform(EntityRegistry& registry, EntityHandle entity)
	{
		if (!registry.has<ChildNode>(entity))
		{
			return TransformD();
		}
		ChildNode& child_node = registry.get<ChildNode>(entity);

		if (child_node.parent == null_entity || registry.valid(child_node.parent))
		{
			return TransformD();
		}

		TransformD local_transform = TransformD();

		if (registry.has<LocalTransform>(entity))
		{
			local_transform = (TransformD)registry.get<LocalTransform>(entity);
		}

		return TransformUtils::transformBy(calcWorldTransform(registry, child_node.parent), local_transform);
	}

	void EditorApplication::update(TimeStep time_step)
	{
		GENESIS_PROFILE_FUNCTION("EditorApplication::update");
		Application::update(time_step);

		this->scene_window->udpate(time_step);

		/*	Hierarchy/Transform resolve system
		*	This system will resolve the world transform for all entites with a world transform that aren't a root entity
		*/

		auto& view = this->editor_registry.view<WorldTransform, ChildNode>();
		for (EntityHandle entity : view)
		{
			auto&[transform, child_node] = view.get<WorldTransform, ChildNode>(entity);
			transform = (WorldTransform)calcWorldTransform(editor_registry, entity);
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
		this->entity_list_window->drawWindow(this->editor_registry);
		this->entity_properties_window->drawWindow(this->editor_registry, this->entity_list_window->getSelected());
		this->scene_window->drawWindow(this->editor_registry);

		this->ui_renderer->endFrame();

		this->legacy_backend->endFrame();
	}
}
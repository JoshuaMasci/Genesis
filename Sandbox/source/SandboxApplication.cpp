#include "Sandbox/SandboxApplication.hpp"

//Hardcode For now
#include "SDL2_Platform.hpp" 
#include "SDL2_Window.hpp" 
#include "OpenglBackend.hpp"
#include "Genesis/LegacyRendering/LegacyImGui.hpp"

#include "Genesis/Rendering/Camera.hpp"

#include "Genesis/Entity/Entity.hpp"

#include "Genesis/Physics/Rigidbody.hpp"
#include "Genesis/Physics/CollisionComponent.hpp"
#include "Genesis/Component/Mesh.hpp"
#include "Genesis/Component/DebugCamera.hpp"
#include "Genesis/Component/PlayerCharacter.hpp"

#include "imgui.h"

namespace Genesis
{
	Node* addNode(GltfNode* node, GltfModel* model)
	{
		Node* new_node = new Node(node->name);
		new_node->setLocalTransform(node->local_transform);

		//if (new_node->getName() != "Bridge_Glass")
		{
			if (node->mesh != nullptr)
			{
				new_node->addComponent<MeshComponent>(node->mesh);
			}
		}
		/*else
		{
			if (node->mesh != nullptr)
			{
				new_node->addComponent<WindowMeshComponent>(node->mesh, WindowMeshComponent::WindowView::ChildWorld);
			}
		}*/

		for (auto child_nodes : node->child_nodes)
		{
			new_node->addChild(addNode(child_nodes, model));
		}

		return new_node;
	}

	Entity* LoadModelToWorld(GltfModel* model)
	{
		Entity* entity = new Entity(12, "Gltf_Model");
		Node* entity_node = entity->getRootNode();

		for (auto root_node : model->root_nodes)
		{
			entity_node->addChild(addNode(root_node, model));
		}

		return entity;
	}

	GltfModel* loadModel(LegacyBackend* backend, const string& filename)
	{
		tinygltf::Model gltfModel;
		tinygltf::TinyGLTF loader;
		Genesis::string error;
		Genesis::string warning;
		bool return_value;

		if (filename.substr(filename.find_last_of(".") + 1) == "gltf")
		{
			return_value = loader.LoadASCIIFromFile(&gltfModel, &error, &warning, filename);
		}
		else if (filename.substr(filename.find_last_of(".") + 1) == "glb")
		{
			return_value = loader.LoadBinaryFromFile(&gltfModel, &error, &warning, filename);
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

		return new Genesis::GltfModel(backend, gltfModel);
	}
}

SandboxApplication::SandboxApplication()
{
	Genesis::Logging::console_sink->setConsoleWindow(&this->console_window);

	this->platform = new Genesis::SDL2_Platform(this);
	this->window = new Genesis::SDL2_Window(Genesis::vector2U(1600, 900), "Sandbox Editor");

	this->legacy_backend = new Genesis::Opengl::OpenglBackend((Genesis::SDL2_Window*) window);
	this->ui_renderer = new Genesis::LegacyImGui(this->legacy_backend, this->input_manager, this->window);

	this->world = new Genesis::World(1);
	this->secondary_world = new Genesis::World(2);

	this->offscreen_size = Genesis::vector2U(1024);
	this->world_renderer = new Genesis::LegacyWorldRenderer(this->legacy_backend, this->offscreen_size);

	{
		this->model = Genesis::loadModel(this->legacy_backend, "res/CesiumMan.gltf");
		Genesis::Entity* entity = LoadModelToWorld(this->model);
		this->secondary_world->addEntity(entity);
	}

	{
		this->outside_model = Genesis::loadModel(this->legacy_backend, "res/Ship_Out.glb");
		Genesis::Entity* entity = LoadModelToWorld(this->outside_model);
		this->world->addEntity(entity);
		entity->setSubworld(secondary_world);
		entity->createRigidbody();
		entity->getRootNode()->createCollisionShape();
		entity->getRootNode()->getCollisionShape()->updateShape(new reactphysics3d::BoxShape(reactphysics3d::Vector3(2.0, 2.0, 6.0)));
	}

	{
		this->inside_model = Genesis::loadModel(this->legacy_backend, "res/Ship_In.glb");
		Genesis::Entity* entity = LoadModelToWorld(this->inside_model);
		this->secondary_world->addEntity(entity);
	}

	{
		Genesis::Entity* camera = new Genesis::Entity(0, "Main Camera");
		camera->setWorldTransform(Genesis::vector3D(0.0, 0.0, -10.0));
		camera->getRootNode()->addComponent<Genesis::DebugCamera>(5.0, 0.3, this->input_manager);
		camera->getRootNode()->addComponent<Genesis::PlayerCharacter>(this->input_manager);
		this->world->addEntity(camera);
		this->world->setActiveCamera(camera->getRootNode());

		/*Genesis::Entity* entity = new Genesis::Entity(0);
		this->world->addEntity(entity);
		entity->createRigidbody();
		entity->getRootNode()->createCollisionShape();
		entity->getRootNode()->getCollisionShape()->updateShape(new reactphysics3d::BoxShape(reactphysics3d::Vector3(2.0, 2.0, 2.0)));

		entity->setWorldTransform(Genesis::TransformD(Genesis::vector3D(0.0, 30.0, 4.0)));
		entity->getRigidbody()->setLinearVelocity(Genesis::vector3D(0.0, -3.0, 0.0));*/

		/*entity = new Genesis::Entity(1);
		this->world->addEntity(entity);
		entity->createRigidbody();
		entity->getRootNode()->createCollisionShape();
		entity->getRootNode()->getCollisionShape()->updateShape(new reactphysics3d::BoxShape(reactphysics3d::Vector3(1.0, 1.0, 1.0)));
		entity->getRootNode()->addComponent<Genesis::MeshComponent>(&this->cube_model->meshes[0]);*/
	}
}

SandboxApplication::~SandboxApplication()
{
	delete this->model;
	delete this->outside_model;

	delete this->world;
	delete this->secondary_world;

	delete this->world_renderer;

	delete this->legacy_backend;

	Genesis::Logging::console_sink->setConsoleWindow(nullptr);
}

void SandboxApplication::update(Genesis::TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("SandboxApplication::update");
	Genesis::Application::update(time_step);

	this->world->update(time_step);
	this->secondary_world->update(time_step);
}

void SandboxApplication::render(Genesis::TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("SandboxApplication::render");

	Application::render(time_step);

	this->legacy_backend->startFrame();
	Genesis::vector4F clear_color = Genesis::vector4F(0.0f, 0.0f, 0.0f, 1.0f);
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
		Genesis::FrameStats stats = this->legacy_backend->getLastFrameStats();
		ImGui::Begin("Stats");
		ImGui::LabelText(std::to_string(time_step * 1000.0).c_str(), "Frame Time (ms)");
		ImGui::LabelText(std::to_string(stats.draw_calls).c_str(), "Draw Calls");
		ImGui::LabelText(std::to_string(stats.triangles_count).c_str(), "Tris count");
		ImGui::End();
	}

	{
		ImGui::Begin("GameView");
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		ImVec2 window_size = ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);

		//Rebuild Framebuffer
		Genesis::vector2U new_framebuffer_size = Genesis::vector2U((uint32_t)window_size.x, (uint32_t)window_size.y);
		if (new_framebuffer_size != this->offscreen_size)
		{
			this->offscreen_size = new_framebuffer_size;
			this->world_renderer->rebuildFramebuffers(this->offscreen_size);
		}

		{
			Genesis::Node* camera_node = world->getActiveCamrea();
			Genesis::Camera camera = Genesis::Camera(95.0f);
			Genesis::TransformD camera_transform = camera_node->getGlobalTransform();
			ImGui::Image((ImTextureID)this->legacy_backend->getFramebufferColorAttachment(this->world_renderer->drawScene(this->world, camera, camera_transform), 0), window_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		}

		ImGui::End();
	}

	this->console_window.drawWindow();
	this->world_view_window.drawWindow(this->world);
	this->entity_properties_window.drawWindow(this->world_view_window.selected_entity_ptr);

	this->ui_renderer->endFrame();

	this->legacy_backend->endFrame();
}

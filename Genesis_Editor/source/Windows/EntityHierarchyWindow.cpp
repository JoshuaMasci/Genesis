#include "Genesis_Editor/Windows/EntityHierarchyWindow.hpp"

#include "imgui.h"

#include "Genesis/Resource/ResourceManager.hpp"

#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Component/TransformComponent.hpp"
#include "Genesis/Component/ModelComponent.hpp"

#include "Genesis/Scene/Hierarchy.hpp"

namespace Genesis
{
	EntityHierarchyWindow::EntityHierarchyWindow(ResourceManager* resource_manager)
	{
		this->resource_manager = resource_manager;
	}

	void worldDragDrop(Scene* scene)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
			{
				GENESIS_ENGINE_ASSERT(payload->DataSize == sizeof(Entity), "Payload Data Size wrong size");
				Entity moved_entity = *(Entity*)payload->Data;

				if (moved_entity.has<ChildNode>())
				{
					ChildNode child_node = moved_entity.get<ChildNode>();
					if (child_node.parent != null_entity)
					{
						Entity(scene, child_node.parent).removeChild(moved_entity);
					}
				}
			}
		}
	}

	void EntityHierarchyWindow::draw(Scene* scene)
	{
		ImGui::Begin("Entity Hierarchy");

		worldDragDrop(scene);

		ImGui::OpenPopupOnItemClick("Create_Entity_Popup", 1);

		scene->registry.each([&](auto entity)
		{
			if (entity != scene->scene_components.handle())
			{
				if (!scene->registry.has<ChildNode>(entity))
				{
					this->drawEntity(scene, Entity(scene, entity));
				}
			}
		});

		ImVec2 im_remaining_space = ImGui::GetContentRegionAvail();

		const int min_empty_height = 50;
		if (im_remaining_space.y < min_empty_height)
		{
			im_remaining_space.y = min_empty_height;
		}

		ImGui::InvisibleButton("Invisible_Drop_Target", im_remaining_space);

		ImGui::OpenPopupOnItemClick("Create_Entity_Popup", 1);

		worldDragDrop(scene);

		if (ImGui::BeginPopup("Create_Entity_Popup"))
		{
			if (ImGui::MenuItem("Create Empty")) 
			{ 
				scene->createEntity("Empty Entity");
			}
			if (ImGui::MenuItem("Create Cube"))
			{
				Entity cube = scene->createEntity("cube");
				cube.add<Transform>();
				cube.add<WorldTransform>();
				cube.add<ModelComponent>(this->resource_manager->mesh_pool.getResource("res/meshes/cube.obj"), make_shared<Material>("No Material"));
			};
			ImGui::EndPopup();
		}

		ImGui::End();
	}

	Entity EntityHierarchyWindow::getSelected()
	{
		return this->selected_entity;
	}

	void EntityHierarchyWindow::drawEntity(Scene* scene, Entity entity)
	{
		const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
		ImGuiTreeNodeFlags node_flags = base_flags;

		if (this->selected_entity == entity)
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		{
			EntityHiearchy hiearchy = entity.children();
			bool has_children = (hiearchy.begin() != hiearchy.end());
			if (!has_children)
			{
				node_flags |= ImGuiTreeNodeFlags_Leaf;
			}
		}

		char* entity_name = "Unnamed Entity";
		if (entity.has<NameComponent>())
		{
			NameComponent& name_component = entity.get<NameComponent>();
			entity_name = name_component.data;
		}

		bool node_opened = ImGui::TreeNodeEx((void*)(intptr_t)entt::to_integral(entity.handle()), node_flags, entity_name);

		//On clicked event
		if (ImGui::IsItemClicked())
		{
			if (this->selected_entity == entity)
			{
				this->selected_entity = Entity();
			}
			else
			{
				this->selected_entity = entity;
			}
		}

		//Drag and Drop start
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ENTITY_TREE_HIERARCHY", &entity, sizeof(Entity));
			ImGui::Text(entity_name);
			ImGui::EndDragDropSource();
		}

		//Drag and Drop end
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
			{
				GENESIS_ENGINE_ASSERT(payload->DataSize == sizeof(Entity), "Payload Data Size wrong size");
				Entity moved_entity = *(Entity*)payload->Data;

				if (entity.inSameScene(moved_entity))
				{
					if (moved_entity.has<ChildNode>())
					{
						ChildNode& child_node = moved_entity.get<ChildNode>();

						if (child_node.parent != null_entity)
						{
							Entity(scene, child_node.parent).removeChild(moved_entity);
						}
					}

					entity.addChild(moved_entity);
				}
			}
		}

		if (ImGui::BeginPopupContextItem(entity_name, ImGuiMouseButton_Right))
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				// TODO: add to delete queue
				entity.destory();
			}
			ImGui::EndPopup();
		}

		if (node_opened)
		{
			if (entity.valid())
			{
				for (EntityHandle child : entity.children())
				{
					this->drawEntity(scene, Entity(scene, child));
				}
			}
			ImGui::TreePop();
		}
	}

	GameObject* createGameObjectMenu()
	{
		if (ImGui::MenuItem("Create Empty"))
		{
			return new GameObject("Empty");
		}

		if (ImGui::MenuItem("Create Cube"))
		{
			return new GameObject("Cube");
			/*Entity cube = scene->createEntity("cube");
			cube.add<Transform>();
			cube.add<WorldTransform>();
			cube.add<ModelComponent>(this->resource_manager->mesh_pool.getResource("res/meshes/cube.obj"), make_shared<Material>("No Material"));*/
		};

		return nullptr;
	}

	void worldDragDrop(GameWorld* world)
	{

	}

	void EntityHierarchyWindow::draw(GameWorld* world)
	{
		ImGui::Begin("Entity Hierarchy");

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
			{
				GENESIS_ENGINE_ASSERT(payload->DataSize == sizeof(GameObject*), "Payload Data Size wrong size");
				this->moved_object = *(GameObject**)payload->Data;
				this->target_world = world;
			}
		}

		ImGui::OpenPopupOnItemClick("Create_Entity_Popup", 1);

		for (GameObject* game_object : world->getGameObjects())
		{
			this->drawGameObject(game_object);
		}

		ImVec2 im_remaining_space = ImGui::GetContentRegionAvail();

		const int min_empty_height = 50;
		if (im_remaining_space.y < min_empty_height)
		{
			im_remaining_space.y = min_empty_height;
		}

		ImGui::InvisibleButton("Invisible_Drop_Target", im_remaining_space);

		ImGui::OpenPopupOnItemClick("Create_Entity_Popup", 1);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
			{
				GENESIS_ENGINE_ASSERT(payload->DataSize == sizeof(GameObject*), "Payload Data Size wrong size");
				this->moved_object = *(GameObject**)payload->Data;
				this->target_world = world;
			}
		}

		if (ImGui::BeginPopup("Create_Entity_Popup"))
		{
			GameObject* new_object = createGameObjectMenu();

			if (new_object)
			{
				world->add_object(new_object);
			}

			ImGui::EndPopup();
		}

		ImGui::End();

		if (this->moved_object)
		{
			GENESIS_ENGINE_ASSERT((this->target_object && !this->target_world) || (!this->target_object && this->target_world), "Invalid move target");
			
			GameObject* old_parent = this->moved_object->get_parent();
			if (old_parent)
			{
				old_parent->remove_child(this->moved_object);
			}
			else
			{
				GameWorld* old_world = this->moved_object->get_world();
				if (old_world)
				{
					old_world->remove_object(this->moved_object);
				}
			}

			if (this->target_object)
			{
				this->target_object->add_child(this->moved_object);
			}
			else if (this->target_world)
			{
				this->target_world->add_object(this->moved_object);
			}

			this->moved_object = nullptr;
			this->target_object = nullptr;
			this->target_world = nullptr;
		}

		if (this->delete_object)
		{
			GameObject* old_parent = this->delete_object->get_parent();
			if (old_parent)
			{
				old_parent->remove_child(this->delete_object);
			}
			else
			{
				GameWorld* old_world = this->delete_object->get_world();
				if (old_world)
				{
					old_world->remove_object(this->delete_object);
				}
			}

			delete this->delete_object;
			this->delete_object = nullptr;
		}
	}

	GameObject* EntityHierarchyWindow::getSelectedObject()
	{
		return this->selected_object;
	}

	void EntityHierarchyWindow::drawGameObject(GameObject* object)
	{
		const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
		ImGuiTreeNodeFlags node_flags = base_flags;

		if (this->selected_object == object)
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		{
			auto& hiearchy = object->get_children();
			bool has_children = (hiearchy.begin() != hiearchy.end());
			if (!has_children)
			{
				node_flags |= ImGuiTreeNodeFlags_Leaf;
			}
		}

		const char* object_name = object->get_name().c_str();
		bool node_opened = ImGui::TreeNodeEx((void*)object, node_flags, object_name);

		//On clicked event
		if (ImGui::IsItemClicked())
		{
			if (this->selected_object == object)
			{
				this->selected_object = nullptr;
			}
			else
			{
				this->selected_object = object;
			}
		}

		//Drag and Drop start
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ENTITY_TREE_HIERARCHY", &object, sizeof(GameObject*));
			ImGui::Text(object->get_name().c_str());
			ImGui::EndDragDropSource();
		}

		//Drag and Drop end
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
			{
				GENESIS_ENGINE_ASSERT(payload->DataSize == sizeof(GameObject*), "Payload Data Size wrong size");
				this->moved_object = *(GameObject**)payload->Data;
				this->target_object = object;
			}
		}

		if (ImGui::BeginPopupContextItem(object_name, ImGuiMouseButton_Right))
		{
			if (ImGui::MenuItem("Delete GameObject"))
			{
				this->delete_object = object;
			}
			ImGui::Separator();

			GameObject* new_object = createGameObjectMenu();

			if (new_object)
			{
				object->add_child(new_object);
			}

			ImGui::EndPopup();
		}

		if (node_opened)
		{
			for (GameObject* child : object->get_children())
			{
				this->drawGameObject(child);
			}
			ImGui::TreePop();
		}
	}
}
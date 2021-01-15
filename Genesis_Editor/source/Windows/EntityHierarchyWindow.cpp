#include "Genesis_Editor/Windows/EntityHierarchyWindow.hpp"

#include "imgui.h"

#include "Genesis/Resource/ResourceManager.hpp"

#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Component/TransformComponent.hpp"
#include "Genesis/Component/ModelComponent.hpp"

#include "Genesis/Scene/Hierarchy.hpp"

#include "Genesis/Scene/ScenePrototype.hpp"

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

	Entity EntityHierarchyWindow::getSelected()
	{
		return this->selected_entity;
	}

	EntityPrototype* create_entity_menu(ResourceManager* resource_manager)
	{
		if (ImGui::MenuItem("Create Empty"))
		{
			return new EntityPrototype("Entity");
		}

		if (ImGui::MenuItem("Create Cube"))
		{
			EntityPrototype* cube = new EntityPrototype("Cube");
			cube->add_component<Transform>();
			cube->add_component<WorldTransform>();
			cube->add_component<ModelComponent>(resource_manager->mesh_pool.getResource("res/meshes/cube.obj"), make_shared<Material>("No Material"));
			return cube;
		};

		return nullptr;
	}

	void EntityHierarchyWindow::draw(ScenePrototype* scene)
	{
		ImGui::Begin("Entity Hierarchy");

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
			{
				GENESIS_ENGINE_ASSERT(payload->DataSize == sizeof(EntityPrototype*), "Payload Data Size wrong size");
				this->moved_entity = *(EntityPrototype**)payload->Data;
				this->target_scene = scene;
			}
		}

		ImGui::OpenPopupOnItemClick("Create_Entity_Popup", 1);

		for (EntityPrototype* entity : scene->get_entities())
		{
			this->draw_entity(entity);
		}

		ImVec2 im_remaining_space = ImGui::GetContentRegionAvail();

		const int min_empty_height = 50;
		if (im_remaining_space.y < min_empty_height)
		{
			im_remaining_space.y = min_empty_height;
		}

		ImGui::InvisibleButton("Invisible_Drop_Target", im_remaining_space);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
			{
				GENESIS_ENGINE_ASSERT(payload->DataSize == sizeof(EntityPrototype*), "Payload Data Size wrong size");
				this->moved_entity = *(EntityPrototype**)payload->Data;
				this->target_scene = scene;
			}
		}

		ImGui::OpenPopupOnItemClick("Create_Entity_Popup", 1);

		if (ImGui::BeginPopup("Create_Entity_Popup"))
		{
			EntityPrototype* new_entity = create_entity_menu(this->resource_manager);

			if (new_entity)
			{
				scene->add_entity(new_entity);
			}
			ImGui::EndPopup();
		}

		ImGui::End();

		if (this->moved_entity)
		{
			GENESIS_ENGINE_ASSERT((this->target_entity && !this->target_scene) || (!this->target_entity && this->target_scene), "Invalid move target");

			EntityPrototype* old_parent = this->moved_entity->get_parent();
			if (old_parent)
			{
				old_parent->remove_child(this->moved_entity);
			}
			else
			{
				ScenePrototype* old_scene = this->moved_entity->get_scene();
				if (old_scene)
				{
					old_scene->remove_entity(this->moved_entity);
				}
			}

			if (this->target_entity)
			{
				this->target_entity->add_child(this->moved_entity);
			}
			else if (this->target_scene)
			{
				this->target_scene->add_entity(this->moved_entity);
			}

			this->moved_entity = nullptr;
			this->target_entity = nullptr;
			this->target_scene = nullptr;
		}

		if (this->delete_entity)
		{
			EntityPrototype* old_parent = this->delete_entity->get_parent();
			if (old_parent)
			{
				old_parent->remove_child(this->delete_entity);
			}
			else
			{
				ScenePrototype* old_scene = this->delete_entity->get_scene();
				if (old_scene)
				{
					old_scene->remove_entity(this->delete_entity);
				}
			}

			delete this->delete_entity;
			this->delete_entity = nullptr;
		}
	}

	EntityPrototype* EntityHierarchyWindow::get_selected()
	{
		return this->selected_entity1;
	}

	void EntityHierarchyWindow::draw_entity(EntityPrototype* entity)
	{
		const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
		ImGuiTreeNodeFlags node_flags = base_flags;

		if (this->selected_entity1 == entity)
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		{
			auto& hiearchy = entity->get_children();
			bool has_children = (hiearchy.begin() != hiearchy.end());
			if (!has_children)
			{
				node_flags |= ImGuiTreeNodeFlags_Leaf;
			}
		}

		const char* entity_name = entity->get_name().c_str();
		bool node_opened = ImGui::TreeNodeEx((void*)entity, node_flags, entity->get_name().c_str());

		//On clicked event
		if (ImGui::IsItemClicked())
		{
			if (this->selected_entity1 == entity)
			{
				this->selected_entity1 = nullptr;
			}
			else
			{
				this->selected_entity1 = entity;
			}
		}

		//Drag and Drop start
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ENTITY_TREE_HIERARCHY", &entity, sizeof(EntityPrototype*));
			ImGui::Text(entity->get_name().c_str());
			ImGui::EndDragDropSource();
		}

		//Drag and Drop end
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
			{
				GENESIS_ENGINE_ASSERT(payload->DataSize == sizeof(EntityPrototype*), "Payload Data Size wrong size");
				this->moved_entity = *(EntityPrototype**)payload->Data;
				this->target_entity = entity;
			}
		}

		if (ImGui::BeginPopupContextItem(entity_name, ImGuiMouseButton_Right))
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				this->delete_entity = entity;
			}
			ImGui::Separator();

			EntityPrototype* new_object = create_entity_menu(this->resource_manager);

			if (new_object)
			{
				entity->add_child(new_object);
			}

			ImGui::EndPopup();
		}

		if (node_opened)
		{
			for (EntityPrototype* child : entity->get_children())
			{
				this->draw_entity(child);
			}
			ImGui::TreePop();
		}
	}
}
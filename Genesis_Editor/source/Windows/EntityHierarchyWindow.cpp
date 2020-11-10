#include "Genesis_Editor/Windows/EntityHierarchyWindow.hpp"

#include "imgui.h"

#include "Genesis/Resource/MeshPool.hpp"
#include "Genesis/Resource/MaterialPool.hpp"
#include "Genesis/Component/ModelComponent.hpp"

namespace Genesis
{
	EntityHierarchyWindow::EntityHierarchyWindow()
	{
	}

	EntityHierarchyWindow::~EntityHierarchyWindow()
	{
	}

	void worldDragDrop(World* world)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
			{
				GENESIS_ENGINE_ASSERT(payload->DataSize == sizeof(Entity*), "Payload Data Size wrong size");
				Entity* moved_entity = *(Entity**)payload->Data;

				if (moved_entity->herarchy.parent != nullptr)
				{
					Entity::removeChild(moved_entity->herarchy.parent, moved_entity);
				}
				else
				{
					World::removeEntity(world, moved_entity);
				}

				World::addEntity(world, moved_entity);
			}
		}
	}

	void EntityHierarchyWindow::draw(World* world, MeshPool* mesh_pool, MaterialPool* material_pool)
	{
		ImGui::Begin("Entity Hierarchy");

		worldDragDrop(world);

		ImGui::OpenPopupOnItemClick("Create_Entity_Popup", 1);

		for (Entity* entity : world->hierarchy.children.copyVector())
		{
			drawEntity(world, entity);
		}

		ImVec2 im_remaining_space = ImGui::GetContentRegionAvail();

		const int min_empty_height = 50;
		if (im_remaining_space.y < min_empty_height)
		{
			im_remaining_space.y = min_empty_height;
		}

		ImGui::InvisibleButton("Invisible_Drop_Target", im_remaining_space);

		ImGui::OpenPopupOnItemClick("Create_Entity_Popup", 1);

		worldDragDrop(world);

		if (ImGui::BeginPopup("Create_Entity_Popup"))
		{
			if (ImGui::MenuItem("Create Empty")) 
			{ 
				Entity* entity = new Entity(0, "New Entity");
				World::addEntity(world, entity);
			}
			if (ImGui::MenuItem("Create Cube"))
			{
				Entity* cube = new Entity(0, "Cube");
				cube->components.add<ModelComponent>(mesh_pool->getResource("res/meshes/cube.obj"), make_shared<Material>("No Material"));
				World::addEntity(world, cube);
			};
			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void EntityHierarchyWindow::drawEntity(World* world, Entity* entity)
	{
		const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
		ImGuiTreeNodeFlags node_flags = base_flags;

		if (this->selected_entity == entity)
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}


		if (entity->herarchy.children.empty())
		{
			node_flags |= ImGuiTreeNodeFlags_Leaf;
		}


		bool node_opened = ImGui::TreeNodeEx(entity, node_flags, entity->name.data);

		//On clicked event
		if (ImGui::IsItemClicked())
		{
			if (this->selected_entity == entity)
			{
				this->selected_entity = nullptr;
			}
			else
			{
				this->selected_entity = entity;
			}
		}

		//Drag and Drop start
		if (ImGui::BeginDragDropSource())
		{
			GENESIS_ENGINE_INFO("Begin Drop {:p}", (void*)entity);
			ImGui::SetDragDropPayload("ENTITY_TREE_HIERARCHY", &entity, sizeof(Entity*));
			ImGui::Text(entity->name.data);
			ImGui::EndDragDropSource();
		}

		//Drag and Drop end
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
			{
				GENESIS_ENGINE_ASSERT(payload->DataSize == sizeof(Entity*), "Payload Data Size wrong size");
				Entity* moved_entity = *(Entity**)payload->Data;
				GENESIS_ENGINE_INFO("End Drop   {:p}", (void*)moved_entity);

				if (moved_entity->herarchy.parent != nullptr)
				{
					Entity::removeChild(moved_entity->herarchy.parent, moved_entity);
				}
				else
				{
					World::removeEntity(world, moved_entity);
				}

				Entity::addChild(entity, moved_entity);
			}
		}

		if (ImGui::BeginPopupContextItem(entity->name.data, ImGuiMouseButton_Right))
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				if (entity->herarchy.parent != nullptr)
				{
					Entity::removeChild(entity->herarchy.parent, entity);
				}
				else
				{
					World::removeEntity(world, entity);
				}

				if (this->selected_entity = entity)
				{
					this->selected_entity = nullptr;
				}

				delete entity;
				entity = nullptr;
			}
			ImGui::EndPopup();
		}

		if (node_opened)
		{
			if (entity != nullptr)
			{
				for (Entity* child : entity->herarchy.children.copyVector())
				{
					drawEntity(world, child);
				}
			}

			ImGui::TreePop();
		}
	}

	Entity* EntityHierarchyWindow::getSelected()
	{
		return this->selected_entity;
	}
}
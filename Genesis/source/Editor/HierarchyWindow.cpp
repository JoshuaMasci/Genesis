#include "Genesis/Editor/HierarchyWindow.hpp"

#include "Genesis/Ecs/NameComponent.hpp"
#include "Genesis/Ecs/Hierarchy.hpp"

#include "Genesis/Entity/Entity.hpp"

#include "imgui.h"

using namespace Genesis;

HierarchyWindow::HierarchyWindow()
{
}

HierarchyWindow::~HierarchyWindow()
{
}

void HierarchyWindow::drawWindow(EcsWorld* world)
{
	ImGui::Begin("World Hierarchy");

	ImGui::SetNextItemOpen(true);
	if (ImGui::TreeNode("World"))
	{
		auto& view = world->entity_registry.view<NameComponent>();
		for (EntityHandle entity : view)
		{
			NameComponent& name_component = view.get<NameComponent>(entity);

			const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			ImGuiTreeNodeFlags node_flags = base_flags;

			if (this->selected_entity == entity)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}

			node_flags |= ImGuiTreeNodeFlags_Leaf;

			if (ImGui::TreeNodeEx((const char*)&name_component.data, node_flags, (const char*)&name_component.data))
			{

				ImGui::TreePop();
			}

			if (ImGui::IsItemClicked())
			{
				this->selected_entity = entity;
			}
		}

		ImGui::TreePop();
	}

	ImGui::End();
}

void HierarchyWindow::drawWindow(World* world)
{
	ImGui::Begin("World Hierarchy");

	//if (ImGui::CollapsingHeader("World", ImGuiTreeNodeFlags_DefaultOpen))
	//{
	ImGui::SetNextItemOpen(true);
	if (ImGui::TreeNode("World"))//TODO World->getName();
	{
		/*if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
			{
				GENESIS_ENGINE_ASSERT_ERROR(payload->DataSize == sizeof(Entity*), "Payload Data Size wrong size");
				Entity* moved_entity = *(Entity**)payload->Data;

				GENESIS_ENGINE_INFO("{} to become a child of World", moved_entity->getName());
				if (moved_entity->isRoot())
				{
					//Do nothing
				}
				else
				{
					moved_entity->getParent()->removeChild(moved_entity);
				}

				world->addEntity(moved_entity);
			}
		}*/

		for (Entity* entity : world->getEntities())
		{
			this->drawEntity(entity);
		}
		ImGui::TreePop();
	}

	ImGui::End();
}

void HierarchyWindow::drawEntityHierarchy(EntityRegistry& registry, EntityHandle entity)
{
	if (entity == entt::null)
	{
		return;
	}

	string entity_name = "Unnamed Entity";

	if (registry.has<NameComponent>(entity))
	{
		entity_name = string((const char*)&registry.get<NameComponent>(entity).data);
	}
	
	const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	ImGuiTreeNodeFlags node_flags = base_flags;

	if (this->selected_entity == entity)
	{
		node_flags |= ImGuiTreeNodeFlags_Selected;
	}

	if (registry.has<ParentNode>(entity))
	{
		if (!registry.get<ParentNode>(entity).child_nodes.empty())
		{
			node_flags |= ImGuiTreeNodeFlags_Leaf;
		}
	}

	if (ImGui::TreeNodeEx(entity_name.c_str(), node_flags, entity_name.c_str()))
	{

		ImGui::TreePop();
	}

}

void HierarchyWindow::drawEntity(Entity* entity)
{
	GENESIS_ENGINE_ASSERT_ERROR(entity != nullptr, "Null Entity");

	const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
	ImGuiTreeNodeFlags node_flags = base_flags;

	if (this->selected_entity_ptr == entity)
	{
		node_flags |= ImGuiTreeNodeFlags_Selected;
	}

	bool node_open = ImGui::TreeNodeEx(entity->getName().c_str(), node_flags, entity->getName().c_str());
	if (ImGui::IsItemClicked())
	{
		if (this->selected_entity_ptr != entity)
		{
			this->selected_entity_ptr = entity;
		}
		else
		{
			this->selected_entity_ptr = nullptr;
		}
	}
	if (node_open)
	{
		ImGui::TreePop();
	}
}

/*void HierarchyWindow::drawWindow(World* world)
{
	ImGui::Begin("World Hierarchy");

	//if (ImGui::CollapsingHeader("World", ImGuiTreeNodeFlags_DefaultOpen))
	//{
	ImGui::SetNextItemOpen(true);
	if (ImGui::TreeNode("World"))//TODO World->getName();
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
			{
				GENESIS_ENGINE_ASSERT_ERROR(payload->DataSize == sizeof(Entity*), "Payload Data Size wrong size");
				Entity* moved_entity = *(Entity**)payload->Data;

				GENESIS_ENGINE_INFO("{} to become a child of World", moved_entity->getName());
				if (moved_entity->isRoot())
				{
					//Do nothing
				}
				else
				{
					moved_entity->getParent()->removeChild(moved_entity);
				}

				world->addEntity(moved_entity);
			}
		}

		for (Entity* entity : world->getEntities())
		{
			this->drawEntityTree(entity);
		}
		ImGui::TreePop();
	}

	ImGui::End();
}*/

/*void HierarchyWindow::drawEntityTree(Entity* entity)
{
	GENESIS_ENGINE_ASSERT_ERROR(entity != nullptr, "Null Entity");

	const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	ImGuiTreeNodeFlags node_flags = base_flags;

	if (this->selected_entity == entity)
	{
		node_flags |= ImGuiTreeNodeFlags_Selected;
	}

	if (entity->getChildren().empty())
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf;
	}

	bool node_open = ImGui::TreeNodeEx(entity->getName().c_str(), node_flags, entity->getName().c_str());
	if (ImGui::IsItemClicked())
	{
		this->selected_entity = entity;
	}

	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("ENTITY_TREE_HIERARCHY", &entity, sizeof(Entity*));
		ImGui::Text(entity->getName().c_str());
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
		{
			GENESIS_ENGINE_ASSERT_ERROR(payload->DataSize == sizeof(Entity*), "Payload Data Size wrong size");
			Entity* moved_entity = *(Entity**)payload->Data;

			GENESIS_ENGINE_INFO("{} to become a child of {}", moved_entity->getName(), entity->getName());
			if (moved_entity->getParent() == nullptr)
			{
				if (moved_entity->getWorld() != nullptr)
				{
					moved_entity->getWorld()->removeEntity(moved_entity);
				}
			}
			else
			{
				moved_entity->getParent()->removeChild(moved_entity);
			}
			
			entity->addChild(moved_entity);
		}
	}

	if (node_open)
	{
		for (Entity* child : entity->getChildren())
		{
			this->drawEntityTree(child);
		}
		ImGui::TreePop();
	}
}*/

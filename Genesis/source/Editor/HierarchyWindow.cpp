#include "Genesis/Editor/HierarchyWindow.hpp"

#include "Genesis/Entity/NameComponent.hpp"

using namespace Genesis;

HierarchyWindow::HierarchyWindow()
{
}

HierarchyWindow::~HierarchyWindow()
{
}

#include "imgui.h"

void HierarchyWindow::drawWindow(World* world)
{
	ImGui::Begin("World Hierarchy");

	ImGui::SetNextItemOpen(true);
	if (ImGui::TreeNode("World"))//TODO World->getName();
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
			{
				GENESIS_ENGINE_ASSERT_ERROR((payload->DataSize == sizeof(Entity*)), "Payload Data Size wrong size");
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
}

void HierarchyWindow::drawEntityTree(Entity* entity)
{
	GENESIS_ENGINE_ASSERT_ERROR((entity != nullptr), "Null Entity");

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
			GENESIS_ENGINE_ASSERT_ERROR((payload->DataSize == sizeof(Entity*)), "Payload Data Size wrong size");
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
}

#include "Genesis/Editor/WorldViewWindow.hpp"

#include "Genesis/Entity/Entity.hpp"

#include "imgui.h"

namespace Genesis
{
	WorldViewWindow::WorldViewWindow()
	{
	}

	WorldViewWindow::~WorldViewWindow()
	{
	}

	void WorldViewWindow::drawWindow(World* world)
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

	void WorldViewWindow::drawEntity(Entity* entity)
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
}
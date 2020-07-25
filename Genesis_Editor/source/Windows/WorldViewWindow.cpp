#include "Genesis_Editor/Windows/WorldViewWindow.hpp"

#include "imgui.h"

#include "Genesis/Component/NameComponent.hpp"

namespace Genesis
{
	WorldViewWindow::WorldViewWindow()
	{
	}

	WorldViewWindow::~WorldViewWindow()
	{
	}

	void WorldViewWindow::drawWindow(EntityRegistry* registry, EntityWorld* world)
	{
		ImGui::Begin("World View");
		
		if (ImGui::CollapsingHeader("World", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto entity : world->getAllEntities())
			{
				const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
				ImGuiTreeNodeFlags node_flags = base_flags;

				if (this->selected_entity == entity)
				{
					node_flags |= ImGuiTreeNodeFlags_Selected;
				}

				bool node_open;
				if (world->hasComponent<NameComponent>(entity))
				{
					NameComponent* name = world->getComponent<NameComponent>(entity);
					node_open = ImGui::TreeNodeEx(name->data, node_flags, name->data);
				}
				else
				{
					string name = "Entity:" + std::to_string(entity);
					node_open = ImGui::TreeNodeEx(name.c_str(), node_flags, name.c_str());
				}

				if (ImGui::IsItemClicked())
				{
					if (this->selected_entity == entity)
					{
						this->selected_entity = InvalidEntity;
					}
					else
					{
						this->selected_entity = entity;
					}
				}

				ImGui::TreePop();
			}

		}
		ImGui::End();
	}
}
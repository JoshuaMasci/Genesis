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
			EntitySignature name_signature = registry->getSignature<NameComponent>();

			for (auto entity : world->getAllEntities())
			{
				const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
				ImGuiTreeNodeFlags node_flags = base_flags;

				if (this->selected_entity == entity.first)
				{
					node_flags |= ImGuiTreeNodeFlags_Selected;
				}

				bool node_open;
				if (world->hasComponent<NameComponent>(entity.first))
				{
					NameComponent* name = world->getComponent<NameComponent>(entity.first);
					node_open = ImGui::TreeNodeEx(name->data, node_flags, name->data);
				}
				else
				{
					string name = "Entity:" + std::to_string(entity.first);
					node_open = ImGui::TreeNodeEx(name.c_str(), node_flags, name.c_str());
				}

				if (ImGui::IsItemClicked())
				{
					if (this->selected_entity == entity.first)
					{
						this->selected_entity = InvalidEntity;
					}
					else
					{
						this->selected_entity = entity.first;
					}
				}

				ImGui::TreePop();
			}

		}
		ImGui::End();
	}
}
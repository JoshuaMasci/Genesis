#include "Genesis_Editor/Windows/EntityListWindow.hpp"

#include "imgui.h"

#include "Genesis/Component/NameComponent.hpp"

#include "Genesis/Component/Hierarchy.hpp"

namespace Genesis
{
	EntityListWindow::EntityListWindow()
	{
	}

	EntityListWindow::~EntityListWindow()
	{
	}

	void EntityListWindow::drawWindow(EntityRegisty& registry)
	{
		ImGui::Begin("Entity List");
		
		if (ImGui::CollapsingHeader("List", ImGuiTreeNodeFlags_DefaultOpen))
		{
			registry.each([&](auto entity)
			{
				const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
				ImGuiTreeNodeFlags node_flags = base_flags;

				if (this->selected_entity == entity)
				{
					node_flags |= ImGuiTreeNodeFlags_Selected;
				}

				char* entity_name = "Unnamed Entity";

				if (registry.has<NameComponent>(entity))
				{
					NameComponent& name_component = registry.get<NameComponent>(entity);
					entity_name = name_component.data;
				}

				if (ImGui::TreeNodeEx(entity_name, node_flags, entity_name))
				{
					ImGui::TreePop();
				}

				if (ImGui::IsItemClicked())
				{
					if (this->selected_entity == entity)
					{
						this->selected_entity = entt::null;
					}
					else
					{
						this->selected_entity = entity;
					}
				}
			});
		}

		if (ImGui::CollapsingHeader("Hierarchy"), ImGuiTreeNodeFlags_DefaultOpen)
		{
			auto view = registry.view<Hierarchy>();

			for (EntityHandle entity : view)
			{
				Hierarchy& hierarchy = view.get<Hierarchy>(entity);

				if (hierarchy.parent == null_entity)
				{
					this->drawEntityTree(registry, entity);
				}
			}
		}


		ImGui::End();
	}

	void EntityListWindow::drawEntityTree(EntityRegisty& registry, EntityHandle entity)
	{
		const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		ImGuiTreeNodeFlags node_flags = base_flags;

		if (this->selected_entity == entity)
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		Hierarchy* hierarchy_component = nullptr;
		if (registry.has<Hierarchy>(entity))
		{
			hierarchy_component = &registry.get<Hierarchy>(entity);
		}

		if (hierarchy_component != nullptr && hierarchy_component->first == null_entity)
		{
			node_flags |= ImGuiTreeNodeFlags_Leaf;
		}

		char* entity_name = "Unnamed Entity";

		if (registry.has<NameComponent>(entity))
		{
			NameComponent& name_component = registry.get<NameComponent>(entity);
			entity_name = name_component.data;
		}

		bool node_opened = ImGui::TreeNodeEx(entity_name, node_flags, entity_name);

		if (ImGui::IsItemClicked())
		{
			if (this->selected_entity == entity)
			{
				this->selected_entity = entt::null;
			}
			else
			{
				this->selected_entity = entity;
			}
		}

		if (node_opened)
		{
			if (hierarchy_component != nullptr)
			{
				EntityHandle child_entity = hierarchy_component->first;

				while (child_entity != null_entity)
				{
					drawEntityTree(registry, child_entity);

					if (registry.has<Hierarchy>(entity))
					{
						child_entity = registry.get<Hierarchy>(entity).next;
					}
					else
					{
						child_entity = null_entity;
					}
				}
			}

			ImGui::TreePop();
		}
	}
}
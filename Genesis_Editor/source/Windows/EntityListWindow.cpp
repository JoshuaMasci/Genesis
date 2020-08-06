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

	void EntityListWindow::drawWindow(EntityRegistry& registry)
	{
		ImGui::Begin("Entity List");
		
		/*if (ImGui::CollapsingHeader("List", ImGuiTreeNodeFlags_DefaultOpen))
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

				if (ImGui::TreeNodeEx(entity_name, node_flags))
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
		}*/

		if (ImGui::CollapsingHeader("Hierarchy", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//Drag and Drop end
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
				{
					GENESIS_ENGINE_ASSERT(payload->DataSize == sizeof(EntityHandle), "Payload Data Size wrong size");
					EntityHandle moved_entity = *(EntityHandle*)payload->Data;

					if (registry.has<ChildNode>(moved_entity))
					{
						ChildNode child_node = registry.get<ChildNode>(moved_entity);
						if (child_node.parent != null_entity)
						{
							Hierarchy::removeChild(registry, child_node.parent, moved_entity);
						}
					}
				}
			}

			auto view = registry.view<ParentNode>(entt::exclude_t<ChildNode>());

			for (EntityHandle entity : view)
			{
				ParentNode& hierarchy = view.get<ParentNode>(entity);
				this->drawEntityTree(registry, entity);
			}
		}

		ImGui::End();
	}

	void EntityListWindow::drawEntityTree(EntityRegistry& registry, EntityHandle entity)
	{
		const ImGuiTreeNodeFlags base_flags = 0;
		ImGuiTreeNodeFlags node_flags = base_flags;

		if (this->selected_entity == entity)
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		ParentNode* parent_node = nullptr;
		if (registry.has<ParentNode>(entity))
		{
			parent_node = &registry.get<ParentNode>(entity);
		}

		if (parent_node == nullptr || (parent_node != nullptr && parent_node->first == null_entity))
		{
			node_flags |= ImGuiTreeNodeFlags_Leaf;
		}

		char* entity_name = "Unnamed Entity";

		if (registry.has<NameComponent>(entity))
		{
			NameComponent& name_component = registry.get<NameComponent>(entity);
			entity_name = name_component.data;
		}

		bool node_opened = ImGui::TreeNodeEx(entity_name, node_flags);

		//On clicked event
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

		//Drag and Drop start
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ENTITY_TREE_HIERARCHY", &entity, sizeof(EntityHandle));
			ImGui::Text(entity_name);
			ImGui::EndDragDropSource();
		}

		//Drag and Drop end
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
			{
				GENESIS_ENGINE_ASSERT(payload->DataSize == sizeof(EntityHandle), "Payload Data Size wrong size");
				EntityHandle moved_entity = *(EntityHandle*)payload->Data;

				if (registry.has<ChildNode>(moved_entity))
				{
					ChildNode child_node = registry.get<ChildNode>(moved_entity);
					if (child_node.parent != null_entity)
					{
						Hierarchy::removeChild(registry, child_node.parent, moved_entity);
					}
				}

				Hierarchy::addChild(registry, entity, moved_entity);
			}
		}

		if (node_opened)
		{
			if (parent_node != nullptr)
			{
				EntityHandle child_entity = parent_node->first;

				while (child_entity != null_entity)
				{
					drawEntityTree(registry, child_entity);

					if (registry.has<ChildNode>(child_entity))
					{
						child_entity = registry.get<ChildNode>(child_entity).next;
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
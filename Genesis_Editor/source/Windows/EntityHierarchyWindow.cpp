#include "Genesis_Editor/Windows/EntityHierarchyWindow.hpp"

#include "imgui.h"

#include "Genesis/Component/NameComponent.hpp"

#include "Genesis/Component/Hierarchy.hpp"

namespace Genesis
{
	EntityHierarchyWindow::EntityHierarchyWindow()
	{
	}

	EntityHierarchyWindow::~EntityHierarchyWindow()
	{
	}

	void EntityHierarchyWindow::draw(EntityRegistry& registry)
	{
		ImGui::Begin("Entity Hierarchy");

		ImGui::OpenPopupOnItemClick("Create_Entity_Popup", 1);

		if (ImGui::BeginPopup("Create_Entity_Popup"))
		{
			if (ImGui::MenuItem("Create Empty")) { registry.create(); }
			ImGui::EndPopup();
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
			}
		}

		auto view = registry.view<ParentNode>(entt::exclude_t<ChildNode>());

		registry.each([&](auto entity)
		{
			if (!registry.has<ChildNode>(entity))
			{
				this->drawEntityTree(registry, entity);
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

		//Drag and Drop end
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY", ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
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

		ImGui::End();
	}

	void EntityHierarchyWindow::drawEntityTree(EntityRegistry& registry, EntityHandle entity)
	{
		const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
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

		bool node_opened = ImGui::TreeNodeEx((void*)(intptr_t)entt::to_integral(entity), node_flags, entity_name);

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
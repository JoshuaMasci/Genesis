#include "Genesis_Editor/Windows/EntityHierarchyWindow.hpp"

#include "imgui.h"

#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Component/Hierarchy.hpp"
#include "Genesis/Component/ModelComponent.hpp"

#include "Genesis/Resource/MeshPool.hpp"
#include "Genesis/Resource/MaterialPool.hpp"

namespace Genesis
{
	EntityHierarchyWindow::EntityHierarchyWindow()
	{
	}

	EntityHierarchyWindow::~EntityHierarchyWindow()
	{
	}

	void EntityHierarchyWindow::draw(EntityWorld* world, MeshPool* mesh_pool, MaterialPool* material_pool)
	{
		ImGui::Begin("Entity Hierarchy");

		ImGui::OpenPopupOnItemClick("Create_Entity_Popup", 1);

		EntityRegistry* registry = world->getRegistry();

		//Drag and Drop end
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
			{
				GENESIS_ENGINE_ASSERT(payload->DataSize == sizeof(EntityHandle), "Payload Data Size wrong size");
				EntityHandle moved_entity = *(EntityHandle*)payload->Data;

				if (registry->has<ChildNode>(moved_entity))
				{
					ChildNode child_node = registry->get<ChildNode>(moved_entity);
					if (child_node.parent != null_entity)
					{
						Hierarchy::removeChild(registry, child_node.parent, moved_entity);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		registry->each([&](auto entity)
		{
			if (!registry->has<ChildNode>(entity))
			{
				this->drawEntityTree(Entity(entity, registry));
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

				if (registry->has<ChildNode>(moved_entity))
				{
					ChildNode child_node = registry->get<ChildNode>(moved_entity);
					if (child_node.parent != null_entity)
					{
						Hierarchy::removeChild(registry, child_node.parent, moved_entity);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginPopup("Create_Entity_Popup"))
		{
			if (ImGui::MenuItem("Create Empty")) { world->createEntity("New Entity"); }
			if (ImGui::MenuItem("Create Cube")) 
			{
				Entity cube = world->createEntity("Cube");
				cube.addComponent<TransformD>();
				cube.addComponent<ModelComponent>(mesh_pool->getResource("res/meshes/cube.obj"), make_shared<Material>("No Material"));
			};
			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void EntityHierarchyWindow::drawEntityTree(Entity entity)
	{
		const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
		ImGuiTreeNodeFlags node_flags = base_flags;

		if (this->selected_entity == entity.getHandle())
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		EntityChildren children = EntityChildren(entity);

		if (children.begin() == children.end())
		{
			node_flags |= ImGuiTreeNodeFlags_Leaf;
		}

		char* entity_name = "Unnamed Entity";

		if (entity.hasComponent<NameComponent>())
		{
			NameComponent& name_component = entity.getComponent<NameComponent>();
			entity_name = name_component.data;
		}

		bool node_opened = ImGui::TreeNodeEx((void*)(intptr_t)entt::to_integral(entity.getHandle()), node_flags, entity_name);

		//On clicked event
		if (ImGui::IsItemClicked())
		{
			if (this->selected_entity == entity.getHandle())
			{
				this->selected_entity = entt::null;
			}
			else
			{
				this->selected_entity = entity.getHandle();
			}
		}

		if (ImGui::BeginPopupContextItem(entity_name, ImGuiMouseButton_Right))
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
			}
			ImGui::EndPopup();
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

				if (moved_entity.getRegistry() == entity.getRegistry())
				{
					if (moved_entity.hasComponent<ChildNode>())
					{
						ChildNode& child_node = moved_entity.getComponent<ChildNode>();

						if (child_node.parent != null_entity)
						{
							Hierarchy::removeChild(moved_entity.getRegistry(), child_node.parent, moved_entity.getHandle());
						}
					}

					Hierarchy::addChild(entity.getRegistry(), entity.getHandle(), moved_entity.getHandle());
				}
			}
		}

		if (node_opened)
		{
			for (Entity child : children)
			{
				drawEntityTree(child);
			}

			ImGui::TreePop();
		}
	}
}
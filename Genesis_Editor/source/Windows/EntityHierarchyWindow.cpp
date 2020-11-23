#include "Genesis_Editor/Windows/EntityHierarchyWindow.hpp"

#include "imgui.h"

#include "Genesis/Resource/MeshPool.hpp"
#include "Genesis/Resource/MaterialPool.hpp"

#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Component/TransformComponent.hpp"
#include "Genesis/Component/ModelComponent.hpp"

#include "Genesis/Ecs/Hierarchy.hpp"

namespace Genesis
{
	EntityHierarchyWindow::EntityHierarchyWindow()
	{
	}

	EntityHierarchyWindow::~EntityHierarchyWindow()
	{
	}

	void worldDragDrop(EntityRegistry& registry)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_TREE_HIERARCHY"))
			{
				GENESIS_ENGINE_ASSERT(payload->DataSize == sizeof(Entity), "Payload Data Size wrong size");
				Entity moved_entity = *(Entity*)payload->Data;

				if (moved_entity.has<ChildNode>())
				{
					ChildNode child_node = moved_entity.get<ChildNode>();
					if (child_node.parent != null_entity)
					{
						HierarchyUtils::removeChild(registry, child_node.parent, moved_entity.handle());
					}
				}
			}
		}
	}

	void EntityHierarchyWindow::draw(EntityWorld* world, MeshPool* mesh_pool, MaterialPool* material_pool)
	{
		EntityRegistry& registry = world->registry;

		ImGui::Begin("Entity Hierarchy");

		worldDragDrop(registry);

		ImGui::OpenPopupOnItemClick("Create_Entity_Popup", 1);

		registry.each([&](auto entity)
		{
			if (!registry.has<ChildNode>(entity))
			{
				this->drawEntity(Entity(&registry, entity));
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

		worldDragDrop(registry);

		if (ImGui::BeginPopup("Create_Entity_Popup"))
		{
			if (ImGui::MenuItem("Create Empty")) 
			{ 
				registry.assign<NameComponent>(registry.create(), "Empty Entity");
			}
			if (ImGui::MenuItem("Create Cube"))
			{
				EntityHandle cube = registry.create();
				registry.assign<NameComponent>(cube, "cube");
				registry.assign<Transform>(cube);
				registry.assign<WorldTransform>(cube);
				registry.assign<ModelComponent>(cube, mesh_pool->getResource("res/meshes/cube.obj"), make_shared<Material>("No Material"));
			};
			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void EntityHierarchyWindow::drawEntity(Entity entity)
	{
		const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
		ImGuiTreeNodeFlags node_flags = base_flags;

		if (this->selected_entity == entity)
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		{
			EntityHiearchy hiearchy = entity.children();
			bool has_children = (hiearchy.begin() != hiearchy.end());
			if (!has_children)
			{
				node_flags |= ImGuiTreeNodeFlags_Leaf;
			}
		}

		char* entity_name = "Unnamed Entity";
		if (entity.has<NameComponent>())
		{
			NameComponent& name_component = entity.get<NameComponent>();
			entity_name = name_component.data;
		}

		bool node_opened = ImGui::TreeNodeEx((void*)(intptr_t)entt::to_integral(entity.handle()), node_flags, entity_name);

		//On clicked event
		if (ImGui::IsItemClicked())
		{
			if (this->selected_entity == entity)
			{
				this->selected_entity = Entity();
			}
			else
			{
				this->selected_entity = entity;
			}
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

				if (moved_entity.registry() == entity.registry())
				{
					if (moved_entity.has<ChildNode>())
					{
						ChildNode& child_node = moved_entity.get<ChildNode>();

						if (child_node.parent != null_entity)
						{
							Entity(moved_entity.registry(), child_node.parent).remove_child(moved_entity);
						}
					}

					entity.add_child(moved_entity);
				}
			}
		}

		if (ImGui::BeginPopupContextItem(entity_name, ImGuiMouseButton_Right))
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				// TODO: add to delete queue
			}
			ImGui::EndPopup();
		}

		if (node_opened)
		{
			for (EntityHandle child : entity.children())
			{
				this->drawEntity(Entity(entity.registry(), child));
			}
			ImGui::TreePop();
		}
	}

	Entity EntityHierarchyWindow::getSelected()
	{
		return this->selected_entity;
	}
}
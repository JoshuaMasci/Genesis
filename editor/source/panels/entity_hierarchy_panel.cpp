#include "genesis_editor/panels/entity_hierarchy_panel.hpp"

#include "imgui.h"

#include "genesis_engine/resource/ResourceManager.hpp"

#include "genesis_engine/component/NameComponent.hpp"
#include "genesis_engine/component/TransformComponent.hpp"
#include "genesis_engine/component/ModelComponent.hpp"

#include "genesis_engine/scene/Hierarchy.hpp"

namespace genesis_engine
{
	EntityHierarchyWindow::EntityHierarchyWindow(ResourceManager* resource_manager)
	{
		this->resource_manager = resource_manager;
	}

	void worldDragDrop(Scene* scene)
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
						Entity(scene, child_node.parent).removeChild(moved_entity);
					}
				}
			}
		}
	}

	void EntityHierarchyWindow::draw(Scene* scene)
	{
		ImGui::Begin("Entity Hierarchy");

		worldDragDrop(scene);

		ImGui::OpenPopupOnItemClick("Create_Entity_Popup", 1);

		scene->registry.each([&](auto entity)
		{
			if (entity != scene->scene_components.handle())
			{
				if (!scene->registry.has<ChildNode>(entity))
				{
					this->drawEntity(scene, Entity(scene, entity));
				}
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

		worldDragDrop(scene);

		if (ImGui::BeginPopup("Create_Entity_Popup"))
		{
			if (ImGui::MenuItem("Create Empty")) 
			{ 
				scene->createEntity("Empty Entity");
			}
			if (ImGui::MenuItem("Create Cube"))
			{
				Entity cube = scene->createEntity("cube");
				cube.add<Transform>();
				cube.add<WorldTransform>();
				cube.add<ModelComponent>(this->resource_manager->mesh_pool.getResource("res/meshes/cube.obj"), make_shared<Material>("No Material"));
			};
			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void EntityHierarchyWindow::drawEntity(Scene* scene, Entity entity)
	{
		const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
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

				if (entity.inSameScene(moved_entity))
				{
					if (moved_entity.has<ChildNode>())
					{
						ChildNode& child_node = moved_entity.get<ChildNode>();

						if (child_node.parent != null_entity)
						{
							Entity(scene, child_node.parent).removeChild(moved_entity);
						}
					}

					entity.addChild(moved_entity);
				}
			}
		}

		if (ImGui::BeginPopupContextItem(entity_name, ImGuiMouseButton_Right))
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				// TODO: add to delete queue
				entity.destory();
			}
			ImGui::EndPopup();
		}

		if (node_opened)
		{
			if (entity.valid())
			{
				for (EntityHandle child : entity.children())
				{
					this->drawEntity(scene, Entity(scene, child));
				}
			}
			ImGui::TreePop();
		}
	}

	Entity EntityHierarchyWindow::get_selected()
	{
		return this->selected_entity;
	}
}
#include "Genesis_Editor/Windows/EntityHierarchyWindow.hpp"

#include "imgui.h"

#include "Genesis/Component/NameComponent.hpp"
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

		registry->each([&](auto entity)
		{
			this->drawEntity(world, Entity(entity, registry));
		});

		ImVec2 im_remaining_space = ImGui::GetContentRegionAvail();

		const int min_empty_height = 50;
		if (im_remaining_space.y < min_empty_height)
		{
			im_remaining_space.y = min_empty_height;
		}

		ImGui::InvisibleButton("Invisible_Drop_Target", im_remaining_space);

		ImGui::OpenPopupOnItemClick("Create_Entity_Popup", 1);

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

	void EntityHierarchyWindow::drawEntity(EntityWorld* world, Entity entity)
	{
		const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
		ImGuiTreeNodeFlags node_flags = base_flags;

		if (this->selected_node == EntityNodeHandle(entity.getHandle(), null_node))
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		if (!entity.hasComponent<NodeComponent>() || entity.getComponent<NodeComponent>().root_children.empty())
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
			if (this->selected_node == EntityNodeHandle(entity.getHandle(), null_node))
			{
				this->selected_node = EntityNodeHandle();
			}
			else
			{
				this->selected_node = EntityNodeHandle(entity.getHandle(), null_node);
			}
		}

		if (ImGui::BeginPopupContextItem(entity_name, ImGuiMouseButton_Right))
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				world->destroyEntity(entity);
			}
			ImGui::EndPopup();
		}

		if (node_opened)
		{
			if (entity.hasComponent<NodeComponent>())
			{
				NodeComponent& node_component = entity.getComponent<NodeComponent>();
				for (NodeHandle child : node_component.root_children)
				{
					this->drawEntityNode(node_component, EntityNodeHandle(entity.getHandle(), child));
				}
			}

			ImGui::TreePop();
		}
	}

	void EntityHierarchyWindow::drawEntityNode(NodeComponent& node_component, EntityNodeHandle entity_node)
	{
		const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;

		if (entity_node.node != null_node)
		{
			Node& node = node_component.registry.get<Node>(entity_node.node);
			ImGuiTreeNodeFlags node_flags = base_flags;

			if (this->selected_node == entity_node)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}

			if (node.children.empty())
			{
				node_flags |= ImGuiTreeNodeFlags_Leaf;
			}

			bool node_opened = ImGui::TreeNodeEx((void*)(intptr_t)entity_node.node, node_flags, node.name.data);

			//On clicked event
			if (ImGui::IsItemClicked())
			{
				if (this->selected_node == entity_node)
				{
					this->selected_node = EntityNodeHandle();
				}
				else
				{
					this->selected_node = entity_node;
				}
			}

			if (node_opened)
			{
				for (NodeHandle child : node.children)
				{
					drawEntityNode(node_component, EntityNodeHandle(entity_node.entity, child));
				}

				ImGui::TreePop();
			}
		}
	}
}
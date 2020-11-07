#include "Genesis_Editor/Windows/EntityHierarchyWindow.hpp"

#include "imgui.h"

#include "Genesis/Component/NameComponent.hpp"


#include "Genesis/Component/ModelComponent.hpp"
#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/CollisionShape.hpp"
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
			this->drawEntity(Entity(registry, entity));
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
				cube.addComponent<RigidBody>();
				CollisionShape& shape = cube.addComponent<CollisionShape>();
				shape.type = CollisionShapeType::Box;
				shape.type_data.box_size = vector3D(0.5);
			};
			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void EntityHierarchyWindow::drawEntity(Entity entity)
	{
		if (!entity.valid())
		{
			return;
		}

		const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
		ImGuiTreeNodeFlags node_flags = base_flags;

		if (this->selected_entity == entity)
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
			if (this->selected_entity == entity)
			{
				this->selected_entity = Entity();
			}
			else
			{
				this->selected_entity = entity;
			}
		}

		if (ImGui::BeginPopupContextItem(entity_name, ImGuiMouseButton_Right))
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				//world->destroyEntity(entity);
			}
			ImGui::EndPopup();
		}

		if (node_opened)
		{
			if (entity.hasComponent<NodeComponent>())
			{
				NodeComponent& node_component = entity.getComponent<NodeComponent>();
				for (EntityHandle child : node_component.root_children)
				{
					this->drawEntityNode(Entity(&node_component.registry, child));
				}
			}

			ImGui::TreePop();
		}
	}

	void EntityHierarchyWindow::drawEntityNode(Entity entity)
	{
		if (!entity.valid())
		{
			return;
		}

		const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
		ImGuiTreeNodeFlags node_flags = base_flags;

		if (!entity.hasComponent<Node>())
		{
			return;
		}

		Node& node = entity.getComponent<Node>();

		if (this->selected_entity == entity)
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}


		if (node.children.empty())
		{
			node_flags |= ImGuiTreeNodeFlags_Leaf;
		}

		char* entity_name = "Unnamed Node";

		if (entity.hasComponent<NameComponent>())
		{
			NameComponent& name_component = entity.getComponent<NameComponent>();
			entity_name = name_component.data;
		}

		bool node_opened = ImGui::TreeNodeEx((void*)(intptr_t)entt::to_integral(entity.getHandle()), node_flags, entity_name);

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

		if (ImGui::BeginPopupContextItem(entity_name, ImGuiMouseButton_Right))
		{
			if (ImGui::MenuItem("Delete Node"))
			{
				//world->destroyEntity(entity);
			}
			ImGui::EndPopup();
		}

		if (node_opened)
		{
			for (EntityHandle child : node.children)
			{
				drawEntityNode(Entity(entity.getRegistry(), child));
			}

			ImGui::TreePop();
		}
	}
}
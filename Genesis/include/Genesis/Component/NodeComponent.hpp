#pragma once

#include <limits>

#include "Genesis/Ecs/Ecs.hpp"
#include "Genesis/Component/NameComponent.hpp"

namespace Genesis
{
	struct Node
	{
		TransformF local_transform;
		TransformF entity_space_transform;

		EntityHandle parent = null_entity;
		vector<EntityHandle> children;
	};

	struct NodeComponent
	{
		EntityRegistry registry;
		vector<EntityHandle> root_children;

		EntityHandle createNode(const char* name = "Node", EntityHandle parent = null_entity)
		{
			EntityHandle node_handle = this->registry.create();
			this->registry.assign<NameComponent>(node_handle, name);
			Node& node = this->registry.assign<Node>(node_handle);
			node.parent = parent;

			Node* parent_node = nullptr;
			if (this->registry.valid(parent) && (parent_node = this->registry.try_get<Node>(parent)) != nullptr)
			{
				parent_node->children.push_back(node_handle);
			}
			else
			{
				this->root_children.push_back(node_handle);
			}

			return node_handle;
		}
	};

	struct NodeSystem
	{
		static void updateTransform(NodeComponent& node_component);
		static TransformF getNodeTransform(NodeComponent& node_component, EntityHandle node_index);
	};
}
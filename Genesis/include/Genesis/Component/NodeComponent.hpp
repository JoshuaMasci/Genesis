#pragma once

#include <limits>

#include "Genesis/Ecs/Ecs.hpp"
#include "Genesis/Component/NameComponent.hpp"

namespace Genesis
{
	struct Node
	{
		NameComponent name;

		TransformF local_transform;
		TransformF entity_space_transform;

		NodeHandle parent = null_node;
		vector<NodeHandle> children;
	};

	struct NodeComponent
	{
		NodeRegistry registry;
		vector<NodeHandle> root_children;

		NodeHandle createNode(const char* name = "Node", NodeHandle parent = null_node)
		{
			NodeHandle node_handle = this->registry.create();
			Node& node = this->registry.assign<Node>(node_handle);
			node.name.set(name);
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
		static TransformF getNodeTransform(NodeComponent& node_component, NodeHandle node_index);
	};
}
#pragma once

#include <limits>

namespace Genesis
{
	typedef uint64_t NodeId;
	const NodeId invalid_node = std::numeric_limits<NodeId>::max();

	struct Node
	{
		TransformF local_transform;
		TransformF entity_space_transform;

		NodeId parent {};
		vector<NodeId> children;
	};

	struct NodeComponent
	{
		vector<NodeId> root_children;
		vector<Node> node_storage;
	};

	struct NodeSystem
	{
		static void updateTransform(NodeComponent& node_component);
		static TransformF getNodeTransform(NodeComponent& node_component, NodeId node_index);
	};
}
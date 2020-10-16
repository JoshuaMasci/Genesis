#include "Genesis/Component/NodeComponent.hpp"

namespace Genesis
{
	void updateNodeTransform(vector<Node>& node_storage, NodeId node_id, const TransformF& parent_transform)
	{
		GENESIS_ENGINE_ASSERT(node_id < node_storage.size(), "NodeId out of range");
		Node& node = node_storage[node_id];
		TransformUtils::transformByInplace(node.entity_space_transform, parent_transform, node.local_transform);

		for (NodeId child : node.children)
		{
			updateNodeTransform(node_storage, child, node.entity_space_transform);
		}
	}

	void NodeSystem::updateTransform(NodeComponent& node_component)
	{
		for (NodeId child : node_component.root_children)
		{
			updateNodeTransform(node_component.node_storage, child, TransformF());
		}
	}

	TransformF NodeSystem::getNodeTransform(NodeComponent& node_component, NodeId node_index)
	{
		return node_component.node_storage[node_index].entity_space_transform;
	}
}
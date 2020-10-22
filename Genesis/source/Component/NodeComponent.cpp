#include "Genesis/Component/NodeComponent.hpp"

namespace Genesis
{
	void updateNodeTransform(NodeComponent& node_component, EntityHandle node_handle, const TransformF& parent_transform)
	{
		Node& node = node_component.registry.get<Node>(node_handle);
		TransformUtils::transformByInplace(node.entity_space_transform, parent_transform, node.local_transform);

		for (EntityHandle child : node.children)
		{
			updateNodeTransform(node_component, child, node.entity_space_transform);
		}
	}

	void NodeSystem::updateTransform(NodeComponent& node_component)
	{
		for (EntityHandle child : node_component.root_children)
		{
			updateNodeTransform(node_component, child, TransformF());
		}
	}

	TransformF NodeSystem::getNodeTransform(NodeComponent& node_component, EntityHandle node_handle)
	{
		return node_component.registry.get<Node>(node_handle).entity_space_transform;
	}
}
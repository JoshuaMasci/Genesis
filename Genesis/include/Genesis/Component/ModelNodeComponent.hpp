#pragma once

#include "Genesis/Component/ModelComponent.hpp"
#include "Genesis/Component/NodeComponent.hpp"

namespace Genesis
{
	struct ModelNode
	{
		NodeId node_index;
		ModelComponent model;
	};

	struct ModelNodeComponent
	{
		vector<ModelNode> models;
	};
}
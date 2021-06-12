#pragma once

#include "genesis_core/rendering/vertex_elements.hpp"

namespace genesis
{
	typedef void* VertexInputDescription;

	struct VertexInputDescriptionCreateInfo
	{
		VertexElementType* input_elements = nullptr;
		uint32_t input_elements_count = 0;
	};

}
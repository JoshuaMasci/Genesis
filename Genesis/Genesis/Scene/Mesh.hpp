#pragma once

#include "Genesis/Core/Transform.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/VertexInputDescription.hpp"

namespace Genesis
{
	struct Mesh
	{
		VertexBuffer vertex_buffer;
		VertexInputDescription* vertex_description;

		IndexBuffer index_buffer;
		IndexType index_type;
		uint32_t index_count;

		Texture texture;
	};
}
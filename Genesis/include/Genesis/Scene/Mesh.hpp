#pragma once

#include "Genesis/Core/Transform.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/VertexInputDescription.hpp"

namespace Genesis
{
	struct Mesh
	{
		StaticBuffer vertex_buffer;
		VertexInputDescription* vertex_description;

		StaticBuffer index_buffer;
		IndexType index_type;
		uint32_t index_count;

		float frustum_sphere_radius;
	};
}
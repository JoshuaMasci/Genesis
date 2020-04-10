#pragma once

#include "Genesis/LegacyBackend/LegacyBackend.hpp"

namespace Genesis
{
	struct LegacyMesh
	{
		VertexBuffer vertex_buffer;
		IndexBuffer index_buffer;
		uint32_t index_count;

		float frustum_sphere_radius;
	};
}
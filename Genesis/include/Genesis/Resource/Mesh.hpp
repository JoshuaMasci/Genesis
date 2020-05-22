#pragma once

//LegacyBackend only for now
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

namespace Genesis
{
	struct Mesh
	{
		VertexBuffer vertex_buffer;
		IndexBuffer index_buffer;
		uint32_t index_count;
	};
}
#pragma once

#include "Game/Chunk.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/Resource/Mesh.hpp"

namespace Genesis
{
	class ChunkMeshGenerator
	{
	protected:
		Genesis::LegacyBackend* backend = nullptr;

	public:
		ChunkMeshGenerator(Genesis::LegacyBackend* backend);

		Genesis::Mesh* generateMesh(const DefaultChunk& chunk);
	};
}
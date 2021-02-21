#pragma once

#include "genesis_engine/LegacyBackend/LegacyBackend.hpp"

#include "genesis_engine/resource/mesh_pool.hpp"
#include "genesis_engine/resource/texture_pool.hpp"
#include "genesis_engine/resource/material_pool.hpp"

#include "genesis_engine/resource/mesh.hpp"
#include "genesis_engine/resource/texture.hpp"
#include "genesis_engine/resource/material.hpp"

namespace genesis_engine
{
	class ResourceManager
	{
	public:
		ResourceManager(LegacyBackend* backend)
		:mesh_pool(backend), texture_pool(backend), material_pool(&this->texture_pool){};

		MeshPool mesh_pool;
		TexturePool texture_pool;
		MaterialPool material_pool;
	};
}
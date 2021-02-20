#pragma once

#include "genesis_engine/LegacyBackend/LegacyBackend.hpp"

#include "genesis_engine/resource/MeshPool.hpp"
#include "genesis_engine/resource/TexturePool.hpp"
#include "genesis_engine/resource/MaterialPool.hpp"

#include "genesis_engine/resource/Mesh.hpp"
#include "genesis_engine/resource/Texture.hpp"
#include "genesis_engine/resource/Material.hpp"

namespace Genesis
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
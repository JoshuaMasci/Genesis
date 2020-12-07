#pragma once

#include "Genesis/LegacyBackend/LegacyBackend.hpp"

#include "Genesis/Resource/MeshPool.hpp"
#include "Genesis/Resource/TexturePool.hpp"
#include "Genesis/Resource/MaterialPool.hpp"

#include "Genesis/Resource/Mesh.hpp"
#include "Genesis/Resource/Texture.hpp"
#include "Genesis/Resource/Material.hpp"

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
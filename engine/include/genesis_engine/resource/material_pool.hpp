#pragma once

#include "genesis_engine/resource/resource_pool.hpp"
#include "genesis_engine/resource/texture_pool.hpp"
#include "genesis_engine/resource/material.hpp"

namespace genesis_engine
{
	class MaterialPool : public ResourcePool<string, Material>
	{
	public:
		MaterialPool(TexturePool* texture_pool);

	protected:
		LegacyBackend* backend = nullptr;
		TexturePool* texture_pool = nullptr;

		virtual shared_ptr<Material> loadResource(const string& key) override;
	};
}
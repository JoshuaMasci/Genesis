#pragma once

#include "genesis_engine/resource/ResourcePool.hpp"
#include "genesis_engine/resource/TexturePool.hpp"
#include "genesis_engine/resource/Material.hpp"

namespace Genesis
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
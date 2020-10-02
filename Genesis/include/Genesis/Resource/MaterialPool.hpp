#pragma once

#include "Genesis/Resource/ResourcePool.hpp"
#include "Genesis/Resource/TexturePool.hpp"
#include "Genesis/Resource/Material.hpp"

namespace Genesis
{
	class MaterialPool : public ResourcePoolNew<string, Material>
	{
	public:
		MaterialPool(TexturePool* texture_pool);

	protected:
		LegacyBackend* backend = nullptr;
		TexturePool* texture_pool = nullptr;

		virtual shared_ptr<Material> loadResource(const string& key) override;
	};
}
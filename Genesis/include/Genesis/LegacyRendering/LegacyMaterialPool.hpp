#pragma once

#include "Genesis/LegacyRendering/LegacyMaterial.hpp"
#include "Genesis/Resource/ResourcePool.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/LegacyRendering/LegacyTexturePool.hpp"

namespace Genesis
{
	class LegacyMaterialPool : public ResourcePool<string, LegacyMaterial>
	{
	public:
		LegacyMaterialPool(LegacyBackend* backend, LegacyTexturePool* pool);
		virtual ~LegacyMaterialPool();

	protected:
		virtual void loadResource(string key) override;
		virtual void unloadResource(string key) override;

		LegacyBackend* legacy_backend;
		LegacyTexturePool* texture_pool;
	};
}
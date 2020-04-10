#pragma once

#include "Genesis/Resource/ResourcePool.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

namespace Genesis
{
	class LegacyTexturePool : public ResourcePool<string, Texture2D>
	{
	public:
		LegacyTexturePool(LegacyBackend* backend);
		virtual ~LegacyTexturePool();

	protected:
		virtual void loadResource(string key) override;
		virtual void unloadResource(string key) override;

		LegacyBackend* legacy_backend;
	};
}
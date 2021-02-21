#pragma once

#include "genesis_engine/resource/resource_pool.hpp"
#include "genesis_engine/LegacyBackend/LegacyBackend.hpp"
#include "genesis_engine/resource/texture.hpp"

namespace genesis_engine
{
	class TexturePool : public ResourcePool<string, Texture>
	{
	public:
		TexturePool(LegacyBackend* backend);

	protected:
		LegacyBackend* backend = nullptr;
		virtual shared_ptr<Texture> loadResource(const string& key) override;
	};
 }
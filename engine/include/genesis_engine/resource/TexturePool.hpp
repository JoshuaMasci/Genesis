#pragma once

#include "genesis_engine/resource/ResourcePool.hpp"
#include "genesis_engine/LegacyBackend/LegacyBackend.hpp"
#include "genesis_engine/resource/Texture.hpp"

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
#pragma once

#include "Genesis/Resource/ResourcePool.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/Resource/Texture.hpp"

namespace Genesis
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
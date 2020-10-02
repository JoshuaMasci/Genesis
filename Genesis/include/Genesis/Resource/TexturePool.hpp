#pragma once

#include "Genesis/Resource/ResourcePool.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"
#include "Genesis/Resource/TextureResource.hpp"

namespace Genesis
{
	class TexturePool : public ResourcePoolNew<string, TextureResource>
	{
	public:
		TexturePool(LegacyBackend* backend);

	protected:
		LegacyBackend* backend = nullptr;
		virtual shared_ptr<TextureResource> loadResource(const string& key) override;
	};
 }
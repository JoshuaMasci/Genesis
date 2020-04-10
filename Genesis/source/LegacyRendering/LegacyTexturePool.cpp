#include "Genesis/LegacyRendering/LegacyTexturePool.hpp"

#include "Genesis/Resource/ResourceLoaders.hpp"

using namespace Genesis;

LegacyTexturePool::LegacyTexturePool(LegacyBackend* backend)
{
	this->legacy_backend = backend;
}

LegacyTexturePool::~LegacyTexturePool()
{
	for (auto resource : this->resources)
	{
		this->unloadResource(resource.first);
	}
}

void LegacyTexturePool::loadResource(string key)
{
	vector2I size;
	int32_t channels;
	uint8_t* data = PngLoader::loadTexture(key, size, channels);

	vector2U unsigned_size = (vector2U)size;

	TextureFormat format;
	switch (channels)
	{
	case 1:
		format = TextureFormat::R;
		break;
	case 2:
		format = TextureFormat::RG;
		break;
	case 3:
		format = TextureFormat::RGB;
		break;
	case 4:
		format = TextureFormat::RGBA;
		break;
	}

	this->resources[key].resource = this->legacy_backend->createTexture(format, unsigned_size, data, 0);
	this->resources[key].using_count = 0;

	PngLoader::unloadTexture(data);
}

void LegacyTexturePool::unloadResource(string key)
{
	ResourceInternal resource = this->resources[key];
	this->legacy_backend->destoryTexture(resource.resource);
}

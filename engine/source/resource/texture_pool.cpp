#include "genesis_engine/resource/texture_pool.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace genesis_engine
{
	uint8_t* loadTexture(const string& texture_file_path, vec2i& size, int32_t& channels)
	{
		return stbi_load(texture_file_path.c_str(), (int*)(&size.x), (int*)(&size.y), (int*)(&channels), STBI_default);
	}

	void unloadTexture(uint8_t* data)
	{
		stbi_image_free(data);
	}

	TexturePool::TexturePool(LegacyBackend* backend)
	{
		this->backend = backend;
	}

	shared_ptr<Texture> TexturePool::loadResource(const string& key)
	{
		Texture2D texture = nullptr;

		//Load image
		vec2i size;
		int32_t channels;
		uint8_t* data = loadTexture(key, size, channels);

		TextureCreateInfo create_info = {};
		create_info.size = (vec2u)size;

		switch (channels)
		{
		case 1:
			create_info.format = ImageFormat::R_8;
			break;
		case 2:
			create_info.format = ImageFormat::RG_8;
			break;
		case 3:
			create_info.format = ImageFormat::RGB_8;
			break;
		case 4:
			create_info.format = ImageFormat::RGBA_8;
			break;
		}

		texture = this->backend->createTexture(create_info, data);
		unloadTexture(data);

		return std::make_shared<Texture>(key, this->backend, texture, create_info.size, create_info.format);
	}
}
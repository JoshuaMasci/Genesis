#pragma once

#include "genesis_engine/resource/Resource.hpp"
#include "genesis_engine/LegacyBackend/LegacyBackend.hpp"

namespace genesis
{
	class Texture : public Resource
	{
	protected:
		LegacyBackend* backend;

	public:
		Texture(const string& file_path, LegacyBackend* backend, Texture2D texture, vec2u size, LegacyImageFormat format)
			:Resource(file_path), backend(backend), texture(texture), size(size), format(format){};

		~Texture()
		{
			this->backend->destoryTexture(this->texture);
		}

		const Texture2D texture;
		const vec2u size;
		const LegacyImageFormat format;
	};
}
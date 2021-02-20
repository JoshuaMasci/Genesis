#pragma once

#include "genesis_engine/resource/Resource.hpp"
#include "genesis_engine/LegacyBackend/LegacyBackend.hpp"

namespace Genesis
{
	class Texture : public Resource
	{
	protected:
		LegacyBackend* backend;

	public:
		Texture(const string& file_path, LegacyBackend* backend, Texture2D texture, vector2U size, ImageFormat format)
			:Resource(file_path), backend(backend), texture(texture), size(size), format(format){};

		~Texture()
		{
			this->backend->destoryTexture(this->texture);
		}

		const Texture2D texture;
		const vector2U size;
		const ImageFormat format;
	};
}
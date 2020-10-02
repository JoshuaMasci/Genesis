#pragma once

#include "Genesis/Resource/Resource.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

namespace Genesis
{
	class TextureResource : public Resource
	{
	protected:
		LegacyBackend* backend;

	public:
		TextureResource(const string& file_path, LegacyBackend* backend, Texture2D texture, vector2U size, ImageFormat format)
			:Resource(file_path), backend(backend), texture(texture), size(size), format(format){};

		~TextureResource()
		{
			this->backend->destoryTexture(this->texture);
		}

		const Texture2D texture;
		const vector2U size;
		const ImageFormat format;
	};
}
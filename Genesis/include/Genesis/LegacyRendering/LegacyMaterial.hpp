#pragma once

#include "Genesis/LegacyBackend/LegacyBackend.hpp"

#include "Genesis/Resource/Material.hpp"

namespace Genesis
{
	struct LegacyMaterial
	{
		static const size_t TextureCount = MaterialTextureCount;

		vector4F albedo;
		vector4F pbr_values;
		string texture_names[MaterialTextureCount];
		Texture2D textures[MaterialTextureCount];
	};
}
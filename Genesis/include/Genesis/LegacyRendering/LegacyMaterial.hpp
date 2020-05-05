#pragma once

#include "Genesis/LegacyBackend/LegacyBackend.hpp"

namespace Genesis
{
	struct LegacyMaterial
	{
		static const size_t TextureCount = 8;

		vector4F albedo;
		vector4F pbr_values;
		string texture_names[TextureCount];
		Texture2D textures[TextureCount];
	};
}
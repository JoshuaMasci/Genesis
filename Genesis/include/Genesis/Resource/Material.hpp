#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/RenderingBackend/RenderingTypes.hpp"
#include "Genesis/RenderingBackend/RenderingBackend.hpp"

#define MaterialTextureCount 8

namespace Genesis
{
	struct MaterialDescription
	{
		const static size_t TextureCount = MaterialTextureCount;

		vector4F albedo;
		float metallic;
		float roughness;
		float ambient_occlusion;
		float height_scale;
		string textures[TextureCount];
	};
}
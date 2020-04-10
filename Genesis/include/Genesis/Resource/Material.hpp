#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/RenderingBackend/RenderingTypes.hpp"
#include "Genesis/RenderingBackend/RenderingBackend.hpp"

#define MaterialTextureCount 8

namespace Genesis
{
	struct MaterialDescription
	{
		vector4F albedo;
		float metallic;
		float roughness;
		float ambient_occlusion;
		float height_scale;
		string textures[MaterialTextureCount];
	};
	
	struct MaterialSettings
	{
		bool transparent = false;
		bool unshaded = false;
		bool no_depth_test = false;
		bool no_shadows = false;
	};

	struct MaterialValuesPacked
	{
		vector4F albedo;
		vector4F pbr_values;
		uint32_t has_textures[MaterialTextureCount];
	};

	struct Material
	{
		static const size_t TextureCount = MaterialTextureCount;

		//MaterialSettings settings;
		StaticBuffer values;
		Texture textures[MaterialTextureCount];

		DescriptorSet material_descriptor;
	};
}
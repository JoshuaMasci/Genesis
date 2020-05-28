#pragma once

#include "Genesis/RenderingBackend/RenderingTypes.hpp"

namespace Genesis
{
	struct PbrMaterial
	{
		//Alpha Info
		enum AlphaMode { ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND };
		AlphaMode alpha_mode = ALPHAMODE_OPAQUE;
		float alpha_cutoff = 1.0f;

		//PBR Factors
		vector4F albedo_factor = vector4F(1.0f);
		vector2F metallic_roughness_factor = vector2F(1.0f);
		vector4F emissive_factor = vector4F(1.0f);

		//PBR Textures
		Texture2D albedo_texture = nullptr;
		Texture2D normal_texture = nullptr;
		Texture2D metallic_roughness_texture = nullptr;
		Texture2D occlusion_texture = nullptr;
		Texture2D emissive_texture = nullptr;

		//PBR Texture UVs
		// -1 means no texture
		//  0 uses mesh uv0
		//  1 uses mesh uv1
		int8_t albedo_uv = -1;
		int8_t normal_uv = -1;
		int8_t metallic_roughness_uv = -1;
		int8_t occlusion_uv = -1;
		int8_t emissive_uv = -1;

		//Material Settings
		bool cull_backface = true;
	};
}
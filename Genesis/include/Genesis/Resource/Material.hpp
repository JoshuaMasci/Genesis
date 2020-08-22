#pragma once

#include "Genesis/RenderingBackend/RenderingTypes.hpp"

namespace Genesis
{
	struct Material
	{
		//values
		vector4F albedo_factor = vector4F(1.0f);
		vector2F metallic_roughness_factor = vector2F(1.0f);
		vector4F emissive_factor = vector4F(1.0f);

		//textures
		Texture2D albedo_texture = nullptr;
		Texture2D normal_texture = nullptr;
		Texture2D metallic_roughness_texture = nullptr;
		Texture2D occlusion_texture = nullptr;
		Texture2D emissive_texture = nullptr;

		//settings
		bool cull_backface = true;
	};
}
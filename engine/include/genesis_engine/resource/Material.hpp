#pragma once

#include "genesis_engine/resource/Resource.hpp"
#include "genesis_engine/resource/Texture.hpp"

namespace genesis_engine
{
	class Material : public Resource
	{
	public:
		struct MaterialTexture
		{
			shared_ptr<Texture> texture;

			//Texture UVs
			// -1 means no texture
			//  0 uses mesh uv
			//  1 uses mesh uv1 (Not Used Yet)
			int8_t uv = -1;
		};

		Material(const string file_path)
		 : Resource(file_path) {};

		//No deconstructor needed right now as the textures will free themselves
		//Might be needed if factors get stored in a buffer

		//values
		vec4f albedo_factor = vec4f(1.0f);
		vec2f metallic_roughness_factor = vec2f(1.0f);
		vec4f emissive_factor = vec4f(0.0f);

		//textures
		MaterialTexture albedo_texture;
		MaterialTexture normal_texture;
		MaterialTexture metallic_roughness_texture;
		MaterialTexture occlusion_texture;
		MaterialTexture emissive_texture;

		//settings
		bool cull_backface = true;
		bool transparent = false;
	};
}
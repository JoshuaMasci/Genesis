#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"

namespace Genesis
{
	struct MaterialSettings
	{
		bool transparent = false;
		bool unshaded = false;
		bool no_depth_test = false;
		bool no_shadows = false;
	};

	struct MaterialValues
	{
		vector4F albedo;
		float metallic;
		float roughness;
		float ambient_occlusion;
		float height_scale = 0.0f;
	};

	struct MaterialMaps
	{
		Texture albedo_map = nullptr;

		Texture normal_map = nullptr;
		Texture height_map = nullptr;

		Texture metallic_map = nullptr;
		Texture roughness_map = nullptr;
		Texture occlusion_map = nullptr;
	};

	struct Material
	{
		MaterialSettings settings;
		MaterialValues values;
		MaterialMaps maps;
	};
}
#pragma once

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	struct TexturedModel
	{
		string mesh;
		string albedo_texture;
	};

	struct TexturedNormalModel
	{
		string mesh;
		string albedo_texture;
		string normal_texture;
		string height_texture;
		float height_scale;
	};
}
#pragma once

#include <cstdint>

namespace Genesis
{
	enum class FilterMode
	{
		Nearest,
		Linear
	};

	enum class AddressMode
	{
		Repeat,
		Mirrored_Repeat,
		Clamp_Edge,
		Clamp_Border
	};

	enum class CompareOp
	{
		Never,
		Less,
		Equal,
		Less_Equal,
		Greater,
		Not_Equal,
		Greater_Equal,
		Always
	};

	enum class BorderColor
	{
		Transparent_Black,
		Opaque_Black,
		Opaque_White
	};

	struct SamplerCreateInfo
	{
		FilterMode min_filter = FilterMode::Nearest;
		FilterMode mag_filter = FilterMode::Nearest;
		FilterMode mipmap_mode = FilterMode::Nearest;
		AddressMode U_address_mode = AddressMode::Repeat;
		AddressMode V_address_mode = AddressMode::Repeat;
		AddressMode W_address_mode = AddressMode::Repeat;
		float mip_lod_bias = 0.0f;
		uint8_t max_anisotropy = 0; //0 is off
		CompareOp compare_op = CompareOp::Never;
		float min_lod = 0.0f;
		float max_lod = 0.0f;
		BorderColor border_color = BorderColor::Transparent_Black;
	};
}
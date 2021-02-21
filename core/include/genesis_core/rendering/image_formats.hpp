#pragma once

namespace genesis_core
{
	enum class ImageFormat
	{
		Invalid,
		RGBA_8_Srgb,
		RGBA_8_Unorm,
		RGBA_8_Snorm,
		R_16_Float,
		RG_16_Float,
		RGB_16_Float,
		RGBA_16_Float,
		R_32_Float,
		RG_32_Float,
		RGB_32_Float,
		RGBA_32_Float,
	};

	enum class DepthStencilFormat
	{
		Invalid,
		S_8_Uint,
		D_16_Unorm,
		D_24_Unorm_S8,
		D_32_Float,
	};
}
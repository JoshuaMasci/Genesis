#pragma once

#include <cstdint>

namespace Genesis
{
	enum class MemoryUsage
	{
		GPU_Only,
		CPU_Visable
	};

	enum class ImageFormat
	{
		Invalid,

		RGBA_8_UNorm,

		R_16_Float,
		RG_16_Float,
		RGB_16_Float,
		RGBA_16_Float,

		R_32_Float,
		RG_32_Float,
		RGB_32_Float,
		RGBA_32_Float,
	};

	typedef void* VertexBufferHandle;
	typedef void* IndexBufferHandle;
	typedef void* UniformBufferHandle;
	typedef void* TextureHandle;
	typedef void* ShaderHandle;

	typedef void* ViewHandle;
	typedef void* CommandBufferHandle;
}
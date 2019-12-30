#pragma once

#include <cstdint>

namespace Genesis
{
	enum class MemoryUsage
	{
		GPU_Only,
		CPU_Visable
	};

	enum class IndexType
	{
		uint16,
		uint32
	};

	enum class ImageFormat
	{
		Invalid,

		RGBA_8_Unorm,

		R_16_Float,
		RG_16_Float,
		RGB_16_Float,
		RGBA_16_Float,

		R_32_Float,
		RG_32_Float,
		RGB_32_Float,
		RGBA_32_Float,

		//Depth Images
		D_16_Unorm,
		D_32_Float,
	};

	typedef void* VertexBuffer;
	typedef void* IndexBuffer;
	typedef void* UniformBuffer;

	typedef void* Texture;
	typedef void* Shader;

	//typedef void* Framebuffer;
	typedef void* View;
}
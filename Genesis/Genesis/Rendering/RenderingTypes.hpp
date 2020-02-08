#pragma once

#include <cstdint>

namespace Genesis
{
	enum class MemoryType
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

	enum class VertexElementType
	{
		//float
		float_1,
		float_2,
		float_3,
		float_4,

		//unorm8
		unorm8_1,
		unorm8_2,
		unorm8_3,
		unorm8_4,

		//uint8
		uint8_1,
		uint8_2,
		uint8_3,
		uint8_4,

		//uint16
		uint16_1,
		uint16_2,
		uint16_3,
		uint16_4,

		//uint32
		uint32_1,
		uint32_2,
		uint32_3,
		uint32_4,
	};

	enum class BufferUsage
	{
		Vertex_Buffer,
		Index_Buffer,
		Uniform_Buffer,
		Storage_Buffer
	};

	typedef void* Sampler;
	typedef void* VertexInputDescription;

	typedef void* StaticBuffer;
	typedef void* DynamicBuffer;

	typedef void* Texture;
	typedef void* Shader;

	typedef void* Framebuffer;
	typedef void* STCommandBuffer;
	typedef void* MTCommandBuffer;

	typedef void* Sampler;
}
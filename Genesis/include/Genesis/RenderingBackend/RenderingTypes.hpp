#pragma once

namespace Genesis
{
	typedef void* VertexBuffer;
	typedef void* IndexBuffer;
	typedef void* Texture2D;
	
	
	//Old Types to be deprecated
	//Waiting on the Modern Rendering Backend refactor
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

	enum class IamgeSamples
	{
		Count_1 = 1,
		Count_2 = 2,
		Count_4 = 4,
		Count_8 = 8,
		Count_16 = 16,
		Count_32 = 32,
		Count_64 = 64,
	};

	enum class BufferUsage
	{
		Vertex_Buffer,
		Index_Buffer,
		Uniform_Buffer,
		Storage_Buffer
	};

	typedef uint32_t ShaderStage;
	enum class ShaderStageBits : ShaderStage
	{
		VERTEX = 0x00000001,
		GEOMETRY = 0x00000008,
		FRAGMENT = 0x00000010,
		COMPUTE = 0x00000020,
	};

	typedef void* StaticBuffer;
	typedef void* DynamicBuffer;

	typedef void* Texture;
	typedef void* Shader;

	typedef void* Framebuffer;
	typedef void* STCommandBuffer;

	typedef void* ShaderModule;
	struct ShaderModuleCreateInfo
	{
		uint32_t* code = nullptr;
		uint32_t code_size = 0;
	};
}
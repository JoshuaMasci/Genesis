#pragma once

#include <cstdint>

namespace Genesis
{
	enum class MemoryUsage
	{
		GPU_Only,
		CPU_Visable
	};

	enum class BufferType
	{
		Uniform,
		Index,
		Vertex
	};

	enum class ViewType
	{
		FrameBuffer,
		ShadowMap
	};

	typedef void* BufferHandle;
	typedef void* TextureHandle;
	typedef void* ViewHandle;
	typedef void* ShaderHandle;
}
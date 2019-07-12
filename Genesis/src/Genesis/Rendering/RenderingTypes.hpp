#pragma once

#include <cstdint>

#define NULL_INDEX 0
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

	typedef uint32_t BufferIndex;
	typedef uint32_t TextureIndex;

	typedef void* ViewIndex;
}
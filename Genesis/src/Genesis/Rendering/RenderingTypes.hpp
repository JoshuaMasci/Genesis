#pragma once

#include <cstdint>

#define NULL_INDEX 0
namespace Genesis
{
	enum class BufferType
	{
		Uniform,
		Index,
		Vertex
	};

	typedef uint32_t BufferIndex;
	typedef uint32_t TextureIndex;
	typedef uint32_t FrameBufferIndex;
	typedef uint32_t ShadowMapIndex;

	const FrameBufferIndex ScreenFrameBuffer = 0;
}
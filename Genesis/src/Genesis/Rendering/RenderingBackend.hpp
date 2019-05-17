#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Rendering/Buffer.hpp"

namespace Genesis
{
	// May need to use a flag system at some point
	// but for now I only exspect to need these types
	enum class BufferType
	{
		Uniform,
		Index,
		Vertex
	};

	enum class MemoryUsage
	{
		GPU_Only,
		CPU_Visable
	};

	class RenderingBackend
	{
	public:
		virtual uint32_t createImage(vector2U size) = 0;
		virtual Buffer* createBuffer(uint32_t size_bytes, BufferType type, MemoryUsage memory_usage) = 0;

	};
}
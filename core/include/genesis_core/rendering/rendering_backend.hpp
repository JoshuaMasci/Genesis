#pragma once

#include "genesis_core/rendering/frame_graph.hpp"

namespace genesis
{
	enum class Temp_BufferUsage
	{
		Vertex,
		Index,
		Uniform,
		Storage
	};

	enum class Temp_MemoryType
	{
		GPU_Only,
		CPU_Visable
	};

	typedef void* BufferHandle;

	class RenderingBackend
	{
	public:
		virtual ~RenderingBackend() = 0;

		virtual BufferHandle create_buffer(size_t buffer_size, Temp_BufferUsage usage, Temp_MemoryType type) = 0;
		virtual void destoy_buffer(BufferHandle buffer) = 0;

		virtual void render(FrameGraph* frame_graph) = 0;
	};
}
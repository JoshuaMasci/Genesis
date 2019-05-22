#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Rendering/Buffer.hpp"

namespace Genesis
{
	enum class MemoryUsage
	{
		GPU_Only,
		CPU_Visable
	};

	class RenderingBackend
	{
	public:
		virtual void beginFrame() = 0;
		virtual void endFrame() = 0;

		virtual Buffer* createBuffer(uint64_t size_bytes, BufferType type, MemoryUsage memory_usage) = 0;
	private:
		uint32_t current_frame = 0;
	};
}
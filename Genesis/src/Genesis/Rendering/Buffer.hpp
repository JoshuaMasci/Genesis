#pragma once

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	// May need to use a flag system at some point
	// but for now I only expect to need these types
	enum class BufferType
	{
		Uniform,
		Index,
		Vertex
	};

	class Buffer
	{
	public:
		virtual ~Buffer() {};
		virtual void fillBuffer(void* data, uint64_t data_size) = 0;
	};
}
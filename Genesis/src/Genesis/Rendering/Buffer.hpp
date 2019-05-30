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

		//Fills the buffer with data
		virtual void fill(void* data, uint64_t data_size) = 0;

		//Gets the API handle to the buffer
		virtual void* getHandle() = 0;
	};
}
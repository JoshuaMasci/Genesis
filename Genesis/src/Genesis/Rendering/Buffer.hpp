#pragma once

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	class Buffer
	{
	public:
		virtual ~Buffer() {};
		virtual void fillBuffer(void* data, uint32_t data_size) = 0;
	};
}
#pragma once

#include "Genesis/Core/Types.hpp"

#include "Genesis/Core/MurmurHash2.hpp"

namespace Genesis
{
	enum UniformType
	{
		Buffer,
		Image,
		Sampler
	};

	struct Binding
	{
		uint32_t index;
		UniformType type;
	};

	//void addSet(uint32_t set_index, )
}
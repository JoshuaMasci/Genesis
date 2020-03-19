#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Debug/Log.hpp"
#include "Genesis/Core/MurmurHash2.hpp"
#include "Genesis/RenderingBackend/RenderingTypes.hpp"

namespace Genesis
{
	typedef void* VertexInputDescription;

	enum class VertexElementType
	{
		//float
		float_1,
		float_2,
		float_3,
		float_4,

		//unorm8
		unorm8_1,
		unorm8_2,
		unorm8_3,
		unorm8_4,

		//uint8
		uint8_1,
		uint8_2,
		uint8_3,
		uint8_4,

		//uint16
		uint16_1,
		uint16_2,
		uint16_3,
		uint16_4,

		//uint32
		uint32_1,
		uint32_2,
		uint32_3,
		uint32_4,
	};

	struct VertexInputDescriptionCreateInfo
	{
		VertexElementType* input_elements = nullptr;
		uint32_t input_elements_count = 0;
	};
}
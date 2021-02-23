#pragma once

namespace genesis
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

	struct VertexElementTypeInfo
	{
		static uint32_t getInputElementSizeByte(VertexElementType type)
		{
			switch (type)
			{
			case VertexElementType::float_1:
				return sizeof(float);
			case VertexElementType::float_2:
				return sizeof(float) * 2;
			case VertexElementType::float_3:
				return sizeof(float) * 3;
			case VertexElementType::float_4:
				return sizeof(float) * 4;
			case VertexElementType::unorm8_1:
				return sizeof(uint8_t);
			case VertexElementType::unorm8_2:
				return sizeof(uint8_t) * 2;
			case VertexElementType::unorm8_3:
				return sizeof(uint8_t) * 3;
			case VertexElementType::unorm8_4:
				return sizeof(uint8_t) * 4;
			case VertexElementType::uint8_1:
				return sizeof(uint8_t);
			case VertexElementType::uint8_2:
				return sizeof(uint8_t) * 2;
			case VertexElementType::uint8_3:
				return sizeof(uint8_t) * 3;
			case VertexElementType::uint8_4:
				return sizeof(uint8_t) * 4;
			case VertexElementType::uint16_1:
				return sizeof(uint16_t);
			case VertexElementType::uint16_2:
				return sizeof(uint16_t) * 2;
			case VertexElementType::uint16_3:
				return sizeof(uint16_t) * 3;
			case VertexElementType::uint16_4:
				return sizeof(uint16_t) * 4;
			case VertexElementType::uint32_1:
				return sizeof(uint32_t);
			case VertexElementType::uint32_2:
				return sizeof(uint32_t) * 2;
			case VertexElementType::uint32_3:
				return sizeof(uint32_t) * 3;
			case VertexElementType::uint32_4:
				return sizeof(uint32_t) * 4;
			default:
				return 0;
			}
		};

		static uint32_t getInputElementCount(VertexElementType type)
		{
			switch (type)
			{
			case VertexElementType::float_1:
			case VertexElementType::unorm8_1:
			case VertexElementType::uint8_1:
			case VertexElementType::uint16_1:
			case VertexElementType::uint32_1:
				return 1;
			case VertexElementType::float_2:
			case VertexElementType::unorm8_2:
			case VertexElementType::uint8_2:
			case VertexElementType::uint16_2:
			case VertexElementType::uint32_2:
				return 2;
			case VertexElementType::float_3:
			case VertexElementType::unorm8_3:
			case VertexElementType::uint8_3:
			case VertexElementType::uint16_3:
			case VertexElementType::uint32_3:
				return 3;
			case VertexElementType::float_4:
			case VertexElementType::unorm8_4:
			case VertexElementType::uint8_4:
			case VertexElementType::uint16_4:
			case VertexElementType::uint32_4:
				return 4;
			}

			return 0;
		};
	};
}
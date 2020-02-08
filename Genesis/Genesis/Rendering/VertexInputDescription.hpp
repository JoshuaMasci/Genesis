/*#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Debug/Log.hpp"
#include "Genesis/Core/MurmurHash2.hpp"
#include "Genesis/Rendering/RenderingTypes.hpp"

namespace Genesis
{
	struct InputElement
	{
		string name;
		VertexElementType type;
	};

	class VertexInputDescription
	{
	public:
		VertexInputDescription()
		{
			//Invalid
			this->hash_value = 0;
			this->total_size = 0;
		};

		VertexInputDescription(vector<InputElement> input_elements)
		{
			uint32_t offset = 0;

			MurmurHash2 hash;
			hash.begin();

			this->elements.resize(input_elements.size());
			for (size_t i = 0; i < this->elements.size(); i++)
			{
				this->elements[i].name = input_elements[i].name;
				this->elements[i].type = input_elements[i].type;
				this->elements[i].offset = offset;
				this->elements[i].size = getInputElementSize(input_elements[i].type);

				offset += this->elements[i].size;

				hash.add(input_elements[i].type);
			}

			this->total_size = offset;

			this->hash_value = hash.end();
		};

		inline size_t getElementCount() { return this->elements.size(); };
		inline string getElementName(size_t i) { return this->elements[i].name; };
		inline VertexElementType getElementType(size_t i) { return this->elements[i].type; };
		inline uint32_t getElementSize(size_t i) { return this->elements[i].size; };
		inline uint32_t getElementOffset(size_t i) { return this->elements[i].offset; };
		inline uint32_t getSize() { return this->total_size; };
		inline uint32_t getHash() { return this->hash_value; };

	private:
		uint32_t getInputElementSize(VertexElementType type)
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
			}

			GENESIS_ENGINE_ERROR("Error: invalid input type");
			assert(false);
			return 0;
		};

		struct InputElementInternal
		{
			string name;
			VertexElementType type;
			uint32_t size;
			uint32_t offset;
		};

		List<InputElementInternal> elements;
		uint32_t total_size = 0;

		uint32_t hash_value;
	};
}*/
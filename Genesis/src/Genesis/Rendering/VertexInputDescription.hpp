#pragma once

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	enum class VertexElementType
	{
		float1,
		float2,
		float3,
		float4,
	};

	struct InputElement
	{
		string name;
		VertexElementType type;
	};

	class VertexInputDescription
	{
	public:
		VertexInputDescription(vector<InputElement> input_elements)
		{
			uint32_t offset = 0;

			this->elements.resize(input_elements.size());
			for (size_t i = 0; i < this->elements.size(); i++)
			{
				this->elements[i].name = input_elements[i].name;
				this->elements[i].type = input_elements[i].type;
				this->elements[i].offset = offset;
				this->elements[i].size = getInputElementSize(input_elements[i].type);

				offset += this->elements[i].size;
			}

			this->total_size = offset;
		};

		inline size_t getNumberOfElements() { return this->elements.size(); };
		inline string getElementName(size_t i) { return this->elements[i].name; };
		inline VertexElementType getElementType(size_t i) { return this->elements[i].type; };
		inline uint32_t getElementSize(size_t i) { return this->elements[i].size; };
		inline uint32_t getElementOffset(size_t i) { return this->elements[i].offset; };
		inline uint32_t getSize() { return this->total_size; };

	private:
		uint32_t getInputElementSize(VertexElementType type)
		{
			switch (type)
			{
			case Genesis::VertexElementType::float1:
				return sizeof(float);
			case Genesis::VertexElementType::float2:
				return sizeof(float) * 2;
			case Genesis::VertexElementType::float3:
				return sizeof(float) * 3;
			case Genesis::VertexElementType::float4:
				return sizeof(float) * 4;
			}

			printf("Error: invalid input type\n");
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

		vector<InputElementInternal> elements;
		uint32_t total_size = 0;
	};
}
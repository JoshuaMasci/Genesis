#include "VulkanVertexInputPool.hpp"

#include "Genesis/Core/MurmurHash2.hpp"
#include "Genesis/RenderingBackend/VertexInputDescription.hpp"

using namespace Genesis;

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
	default:
		return 0;
	}
};

VkFormat getVulkanType(VertexElementType type)
{
	switch (type)
	{
	case VertexElementType::float_1:
		return VK_FORMAT_R32_SFLOAT;
	case VertexElementType::float_2:
		return VK_FORMAT_R32G32_SFLOAT;
	case VertexElementType::float_3:
		return VK_FORMAT_R32G32B32_SFLOAT;
	case VertexElementType::float_4:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	case VertexElementType::unorm8_1:
		return VK_FORMAT_R8_UNORM;
	case VertexElementType::unorm8_2:
		return VK_FORMAT_R8G8_UNORM;
	case VertexElementType::unorm8_3:
		return VK_FORMAT_R8G8B8_UNORM;
	case VertexElementType::unorm8_4:
		return VK_FORMAT_R8G8B8A8_UNORM;
	case VertexElementType::uint8_1:
		return VK_FORMAT_R8_UINT;
	case VertexElementType::uint8_2:
		return VK_FORMAT_R8G8_UINT;
	case VertexElementType::uint8_3:
		return VK_FORMAT_R8G8B8_UINT;
	case VertexElementType::uint8_4:
		return VK_FORMAT_R8G8B8A8_UINT;
	case VertexElementType::uint16_1:
		return VK_FORMAT_R16_UINT;
	case VertexElementType::uint16_2:
		return VK_FORMAT_R16G16_UINT;
	case VertexElementType::uint16_3:
		return VK_FORMAT_R16G16B16_UINT;
	case VertexElementType::uint16_4:
		return VK_FORMAT_R16G16B16A16_UINT;
	case VertexElementType::uint32_1:
		return VK_FORMAT_R32_UINT;
	case VertexElementType::uint32_2:
		return VK_FORMAT_R32G32_UINT;
	case VertexElementType::uint32_3:
		return VK_FORMAT_R32G32B32_UINT;
	case VertexElementType::uint32_4:
		return VK_FORMAT_R32G32B32A32_UINT;
	default:
		return VK_FORMAT_UNDEFINED;
	}
}

VulkanVertexInputPool::VulkanVertexInputPool()
{

}

VulkanVertexInputPool::~VulkanVertexInputPool()
{

}

VulkanVertexInputDescription* VulkanVertexInputPool::getVertexInputDescription(vector<VertexElementType>& input_elements)
{
	MurmurHash2 hash;
	hash.begin();
	hash.addData((uint8_t*)input_elements.data(), (uint32_t)(sizeof(VertexElementType) * input_elements.size()));
	uint32_t hash_value = hash.end();

	VulkanVertexInputDescription* return_description = nullptr;

	this->map_lock.lock();
	if (!has_value(this->input_descriptions, hash_value))
	{
		return_description = &this->input_descriptions[hash_value];

		uint32_t offset = 0;

		return_description->attribute_descriptions.resize(input_elements.size());
		for (uint32_t i = 0; i < return_description->attribute_descriptions.size(); i++)
		{
			return_description->attribute_descriptions[i].binding = 0;
			return_description->attribute_descriptions[i].location = i;
			return_description->attribute_descriptions[i].format = getVulkanType(input_elements[i]);
			return_description->attribute_descriptions[i].offset = offset;
			offset += getInputElementSize(input_elements[i]);
		}

		return_description->binding_description.binding = 0;
		return_description->binding_description.stride = offset;//Total Size
		return_description->binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	return_description = &this->input_descriptions[hash_value];
	this->map_lock.unlock();

	return return_description;
}

VulkanVertexInputDescription* VulkanVertexInputPool::getVertexInputDescription(const VertexInputDescriptionCreateInfo& create_info)
{
	MurmurHash2 hash;
	hash.begin();
	hash.addData((uint8_t*)create_info.input_elements, (uint32_t)(sizeof(VertexElementType) * create_info.input_elements_count));
	uint32_t hash_value = hash.end();
	

	VulkanVertexInputDescription* return_description = nullptr;

	this->map_lock.lock();
	if (!has_value(this->input_descriptions, hash_value))
	{
		return_description = &this->input_descriptions[hash_value];

		uint32_t offset = 0;

		return_description->attribute_descriptions.resize(create_info.input_elements_count);
		for (uint32_t i = 0; i < return_description->attribute_descriptions.size(); i++)
		{
			return_description->attribute_descriptions[i].binding = 0;
			return_description->attribute_descriptions[i].location = i;
			return_description->attribute_descriptions[i].format = getVulkanType(create_info.input_elements[i]);
			return_description->attribute_descriptions[i].offset = offset;
			offset += getInputElementSize(create_info.input_elements[i]);
		}

		return_description->binding_description.binding = 0;
		return_description->binding_description.stride = offset;//Total Size
		return_description->binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	return_description = &this->input_descriptions[hash_value];
	this->map_lock.unlock();

	return return_description;
}

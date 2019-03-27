#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "Genesis/Core/Types.hpp"
#include "Genesis/Core/Array.hpp"

namespace Genesis
{
	struct Vertex
	{
		vector3F pos;
		vector3F color;
		vector2F texCoord;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static Container::Array<VkVertexInputAttributeDescription> getAttributeDescriptions()
		{
			Container::Array<VkVertexInputAttributeDescription> attributeDescriptions(3);

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			return attributeDescriptions;
		}
	};

	class VulkanModel
	{
	public:
		VulkanModel(vector<Vertex> vertices, vector<uint32_t> indices, VmaAllocator& allocator);
		~VulkanModel();

	private:
		VmaAllocator* allocator;

		VkBuffer vertex_buffer;
		VmaAllocation vertex_buffer_memory;

		VkBuffer index_buffer;
		VmaAllocation index_buffer_memory;

		size_t number_of_indices;
	};
}
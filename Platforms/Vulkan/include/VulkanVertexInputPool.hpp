#pragma once

#include "Genesis/RenderingBackend/RenderingTypes.hpp"
#include "Genesis/RenderingBackend/VertexInputDescription.hpp"
#include "VulkanInclude.hpp"
#include "Genesis/Core/Types.hpp"

#include <mutex>

namespace Genesis
{
	struct VulkanVertexInputDescription
	{
		VkVertexInputBindingDescription binding_description;
		vector<VkVertexInputAttributeDescription> attribute_descriptions;
	};

	class VulkanVertexInputPool
	{
	public:
		VulkanVertexInputPool();
		~VulkanVertexInputPool();

		VulkanVertexInputDescription* getVertexInputDescription(vector<VertexElementType>& input_elements);
		VulkanVertexInputDescription* getVertexInputDescription(const VertexInputDescriptionCreateInfo& create_info);

	private:
		VkDevice device;

		std::mutex map_lock;
		map<uint32_t, VulkanVertexInputDescription> input_descriptions;
	};
}
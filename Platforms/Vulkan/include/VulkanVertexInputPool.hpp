#pragma once

#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Rendering/VertexInputDescription.hpp"
#include "VulkanInclude.hpp"
#include "Genesis/Core/Types.hpp"

#include <mutex>

namespace Genesis
{
	struct VulkanVertexInputDescription
	{
		VkVertexInputBindingDescription binding_description;
		List<VkVertexInputAttributeDescription> attribute_descriptions;
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
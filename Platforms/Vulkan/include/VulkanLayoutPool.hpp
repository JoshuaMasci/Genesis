#pragma once

#include "VulkanInclude.hpp"
#include "Genesis/Core/Types.hpp"
#include "Genesis/RenderingBackend/DescriptorSet.hpp"
#include "Genesis/RenderingBackend/PipelineLayout.hpp"

#include <mutex>

namespace Genesis
{
	class VulkanLayoutPool
	{
	public:
		VulkanLayoutPool(VkDevice device);
		~VulkanLayoutPool();

		VkDescriptorSetLayout getDescriptorLayout(List<VkDescriptorSetLayoutBinding>& bindings);
		VkDescriptorSetLayout getDescriptorLayout(const DescriptorSetLayoutCreateInfo& create_info);

		VkPipelineLayout getPipelineLayout(List<VkDescriptorSetLayout>& layouts, List<VkPushConstantRange>& ranges);
		VkPipelineLayout getPipelineLayout(const PipelineLayoutCreateInfo& create_info);

	private:
		VkDevice device;

		std::mutex descriptor_map_lock;
		map<uint32_t, VkDescriptorSetLayout> descriptor_layouts;

		std::mutex pipeline_map_lock;
		map<uint32_t, VkPipelineLayout> pipeline_layouts;
	};
}
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

		VkDescriptorSetLayout getDescriptorLayout(vector<VkDescriptorSetLayoutBinding>& bindings);
		VkDescriptorSetLayout getDescriptorLayout(const DescriptorSetLayoutCreateInfo& create_info);

		VkPipelineLayout getPipelineLayout(vector<VkDescriptorSetLayout>& layouts, vector<VkPushConstantRange>& ranges);
		VkPipelineLayout getPipelineLayout(const PipelineLayoutCreateInfo& create_info);

	private:
		VkDevice device;

		std::mutex render_pass_map_lock;
		map<uint32_t, VkDescriptorSetLayout> descriptor_layouts;

		std::mutex pipeline_map_lock;
		map<uint32_t, VkPipelineLayout> pipeline_layouts;
	};
}
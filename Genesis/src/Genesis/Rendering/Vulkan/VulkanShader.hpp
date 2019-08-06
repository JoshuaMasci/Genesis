#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDescriptorSetLayouts.hpp"

#include "spirv_reflect.h"

namespace Genesis
{
	struct DescriptorSetInfo
	{
		uint32_t index;
		VkDescriptorType type;
		uint32_t size = 0; //For buffers only
		VkPipelineStageFlags shader_stage;
	};

	class VulkanShader
	{
	public:
		VulkanShader(VkDevice device, string vert_data, string frag_data, VulkanDescriptorSetLayouts* layouts);
		~VulkanShader();

		inline vector<VkPipelineShaderStageCreateInfo> getShaderStages() { return this->shader_stages; };
		inline VkPipelineLayout getPipelineLayout() { return this->pipeline_layout; };

	private:
		VkDevice device;
		VkShaderModule createShaderModule(string shader_code);
		void extractLayout(string shader_code, VulkanDescriptorSetLayouts* layouts);

		map<string, DescriptorSetInfo> descriptor_map;

		vector<VkDescriptorSetLayout> descriptor_set_layouts;
		VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;

		VkShaderModule vertShaderModule = VK_NULL_HANDLE;
		VkShaderModule fragShaderModule = VK_NULL_HANDLE;

		vector<VkPipelineShaderStageCreateInfo> shader_stages;
	};
}
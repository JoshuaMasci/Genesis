#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "spirv_reflect.h"

namespace Genesis
{
	class VulkanShader
	{
	public:
		VulkanShader(VkDevice device, string vert_data, string frag_data);
		~VulkanShader();

		inline vector<VkPipelineShaderStageCreateInfo> getShaderStages() { return this->shaderStages; };

	private:
		VkDevice device;
		VkShaderModule createShaderModule(string shader_code);
		void extractLayout(string shader_code);

		vector<VkDescriptorSetLayout> descriptor_set_layouts;
		vector<VkPushConstantRange> push_constant_layouts;
		VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;

		VkShaderModule vertShaderModule = VK_NULL_HANDLE;
		VkShaderModule fragShaderModule = VK_NULL_HANDLE;

		vector<VkPipelineShaderStageCreateInfo> shaderStages;
	};
}
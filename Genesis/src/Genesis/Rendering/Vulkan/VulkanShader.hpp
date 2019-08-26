#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"

namespace Genesis
{
	class VulkanShaderModule
	{
	public:
		VulkanShaderModule(VkDevice device, string shader_data);
		~VulkanShaderModule();

		VkPipelineShaderStageCreateInfo getStageInfo();

		VkShaderModule shader_module;
		Array<VkDescriptorSetLayoutBinding> bindings;
		VkShaderStageFlagBits shader_stage;



	protected:
		VkDevice device;
	};


	class VulkanShader
	{
	public:
		VulkanShader(VkDevice device, string vert_data, string frag_data);
		~VulkanShader();

		inline vector<VkPipelineShaderStageCreateInfo> getShaderStages() { return this->shader_stages; };
		inline VkDescriptorSetLayout getDescriptorSetLayout() { return this->descriptor_layout; };
		inline VkPipelineLayout getPipelineLayout() { return this->pipeline_layout; };

	private:
		VkDevice device;

		VulkanShaderModule* vert_module = nullptr;
		VulkanShaderModule* frag_module = nullptr;

		VkDescriptorSetLayout descriptor_layout = VK_NULL_HANDLE;
		VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
		vector<VkPipelineShaderStageCreateInfo> shader_stages;
	};
}
#pragma once

#include "Genesis/Core/Types.hpp"
#include "VulkanInclude.hpp"

#include "VulkanLayoutPool.hpp"

namespace Genesis
{
	struct DescriptorSetBindingModule
	{
		VkDescriptorType type;
		uint32_t count = 0;
		uint64_t size = 0;
	};

	struct DescriptorSetBinding
	{
		VkDescriptorType type;
		VkShaderStageFlags stage;
		uint32_t count = 0;
		uint64_t size = 0;
	};

	class VulkanShaderModule
	{
	public:
		VulkanShaderModule(VkDevice device, string& shader_data);
		~VulkanShaderModule();

		VkPipelineShaderStageCreateInfo getStageInfo();

		VkShaderModule shader_module;
		VkShaderStageFlagBits shader_stage;

		List<List<DescriptorSetBindingModule>> descriptor_sets;

		List<VkPushConstantRange> push_constant;

	protected:
		VkDevice device;
	};

	class VulkanShader
	{
	public:
		VulkanShader(VkDevice device, string& vert_data, string& frag_data, VulkanLayoutPool* layout_pool);
		~VulkanShader();

		inline vector<VkPipelineShaderStageCreateInfo> getShaderStages() { return this->shader_stages; };
		inline VkPipelineLayout getPipelineLayout() { return this->pipeline_layout; };

		inline const List<List<DescriptorSetBinding>>& getDescriptorSetBindings() { return this->descriptor_set_bindings; };
		inline const List<VkDescriptorSetLayout>& getDescriptorSetLayouts() { return this->descriptor_layouts; };

	private:
		VkDevice device;

		VulkanShaderModule* vert_module = nullptr;
		VulkanShaderModule* frag_module = nullptr;

		List<List<DescriptorSetBinding>> descriptor_set_bindings;

		List<VkDescriptorSetLayout> descriptor_layouts;
		VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;

		vector<VkPipelineShaderStageCreateInfo> shader_stages;
	};
}
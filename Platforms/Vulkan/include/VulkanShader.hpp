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

	struct VulkanDescriptorSetBinding
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
		VulkanShaderModule(VkDevice device, const ShaderModuleCreateInfo& create_info);

		~VulkanShaderModule();

		VkPipelineShaderStageCreateInfo getStageInfo();

		VkShaderModule shader_module;
		VkShaderStageFlagBits shader_stage;

		vector<vector<DescriptorSetBindingModule>> descriptor_sets;

		vector<VkPushConstantRange> push_constant;

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

		inline const vector<vector<VulkanDescriptorSetBinding>>& getDescriptorSetBindings() { return this->descriptor_set_bindings; };
		inline const vector<VkDescriptorSetLayout>& getDescriptorSetLayouts() { return this->descriptor_layouts; };

	private:
		VkDevice device;

		VulkanShaderModule* vert_module = nullptr;
		VulkanShaderModule* frag_module = nullptr;

		vector<vector<VulkanDescriptorSetBinding>> descriptor_set_bindings;

		vector<VkDescriptorSetLayout> descriptor_layouts;
		VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;

		vector<VkPipelineShaderStageCreateInfo> shader_stages;
	};
}
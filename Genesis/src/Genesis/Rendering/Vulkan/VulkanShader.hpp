#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"

namespace Genesis
{
	struct ShaderBindingVariable
	{
		string name;
		uint32_t offset;
		uint32_t size;
		uint32_t padded_size;
	};

	struct ShaderBinding
	{
		string name;
		uint32_t binding_location;
		VkDescriptorType type;
		VkShaderStageFlagBits shader_stage;

		//buffer info
		uint32_t total_size;
		Array<ShaderBindingVariable> variables;

		//Other
		uint32_t array_count;
	};

	struct ShaderBindingVariableLocation
	{
		uint32_t binding;
		uint32_t variable_size; 
		uint32_t variable_offset;
	};

	struct ShaderPushConstant
	{
		string name;
		VkShaderStageFlagBits shader_stage;

		//buffer info
		uint32_t total_size;
		Array<ShaderBindingVariable> variables;
	};

	struct ShaderPushConstantVariableLocation
	{
		VkShaderStageFlagBits variable_stage;
		uint32_t variable_size;
		uint32_t variable_offset;
	};

	class VulkanShaderModule
	{
	public:
		VulkanShaderModule(VkDevice device, string shader_data);
		~VulkanShaderModule();

		VkPipelineShaderStageCreateInfo getStageInfo();

		VkShaderModule shader_module;
		Array<ShaderBinding> shader_bindings;
		VkShaderStageFlagBits shader_stage;

		ShaderPushConstant push_constant;

	protected:
		VkDevice device;
	};

	enum class ShaderVariableType
	{
		Binding,
		PushConstant
	};

	class VulkanShader
	{
	public:
		VulkanShader(VkDevice device, string vert_data, string frag_data);
		~VulkanShader();

		inline vector<VkPipelineShaderStageCreateInfo> getShaderStages() { return this->shader_stages; };
		inline VkDescriptorSetLayout getDescriptorSetLayout() { return this->descriptor_layout; };
		inline VkPipelineLayout getPipelineLayout() { return this->pipeline_layout; };

		ShaderVariableType getVariableType(string name);
		ShaderBindingVariableLocation getBindingVariableLocation(string name);
		ShaderPushConstantVariableLocation getPushConstantVariableLocation(string name);

		map<string, ShaderBinding*> name_bindings;
		map<string, ShaderPushConstant*> name_constants;

	private:
		VkDevice device;

		VulkanShaderModule* vert_module = nullptr;
		VulkanShaderModule* frag_module = nullptr;

		VkDescriptorSetLayout descriptor_layout = VK_NULL_HANDLE;
		VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
		vector<VkPipelineShaderStageCreateInfo> shader_stages;

		map<string, ShaderBindingVariableLocation> name_binding_location_cache;
		map<string, ShaderPushConstantVariableLocation> name_constant_loaction_cache;
	};
}
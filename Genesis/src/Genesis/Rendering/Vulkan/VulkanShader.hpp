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
	};


	struct ShaderPushConstant
	{
		string name;
		VkShaderStageFlagBits shader_stage;

		//buffer info
		uint32_t total_size;
		List<ShaderBindingVariable> variables;
	};

	enum class ShaderVariableType
	{
		Binding,
		PushConstant
	};

	struct ShaderVariableLocation
	{
		ShaderVariableType type;
		uint32_t variable_size;
		uint32_t variable_offset;

		union
		{
			uint32_t binding_index;
			VkShaderStageFlagBits variable_stage;
		} location;
	};

	class VulkanShaderModule
	{
	public:
		VulkanShaderModule(VkDevice device, string& shader_data);
		~VulkanShaderModule();

		void addVariables(map<string, ShaderVariableLocation>& variable_loactions);

		VkPipelineShaderStageCreateInfo getStageInfo();

		VkShaderModule shader_module;
		List<ShaderBinding> shader_bindings;
		VkShaderStageFlagBits shader_stage;

		ShaderPushConstant push_constant;

		map<string, ShaderVariableLocation> variable_loactions;

	protected:
		VkDevice device;
	};

	class VulkanShader
	{
	public:
		VulkanShader(VkDevice device, string& vert_data, string& frag_data);
		~VulkanShader();

		inline vector<VkPipelineShaderStageCreateInfo> getShaderStages() { return this->shader_stages; };
		inline VkDescriptorSetLayout getDescriptorSetLayout() { return this->descriptor_layout; };
		inline VkPipelineLayout getPipelineLayout() { return this->pipeline_layout; };


		ShaderVariableLocation getVariableLocation(string name);

		map<string, ShaderBinding*> name_bindings;
		map<string, ShaderPushConstant*> name_constants;

	private:
		VkDevice device;

		VulkanShaderModule* vert_module = nullptr;
		VulkanShaderModule* frag_module = nullptr;

		VkDescriptorSetLayout descriptor_layout = VK_NULL_HANDLE;
		VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
		vector<VkPipelineShaderStageCreateInfo> shader_stages;

		map<string, ShaderVariableLocation> name_variable_loaction_cache;
	};
}
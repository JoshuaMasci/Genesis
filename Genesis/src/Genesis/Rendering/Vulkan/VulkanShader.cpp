#include "VulkanShader.hpp"

#include <fstream>

using namespace Genesis;

VkShaderModule VulkanShader::createShaderModule(string shader_code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shader_code.length();
	createInfo.pCode = (uint32_t*)shader_code.data();

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create shader module!");
	}

	this->extractLayout(shader_code);

	return shaderModule;
}

void VulkanShader::extractLayout(string shader_code)
{
	SpvReflectShaderModule module;
	SpvReflectResult result = spvReflectCreateShaderModule(shader_code.length(), (uint32_t*)shader_code.data(), &module);

	//Descriptor Sets
	{
		uint32_t count = 0;
		spvReflectEnumerateDescriptorSets(&module, &count, NULL);
		vector<SpvReflectDescriptorSet*> descriptor_sets(count);
		spvReflectEnumerateDescriptorSets(&module, &count, descriptor_sets.data());
		for (size_t descriptor_set_index = 0; descriptor_set_index < descriptor_sets.size(); descriptor_set_index++)
		{
			SpvReflectDescriptorSet* descriptor_set = descriptor_sets[descriptor_set_index];

			vector<VkDescriptorSetLayoutBinding> binding_layouts;

			for (size_t binding_index = 0; binding_index < descriptor_set->binding_count; binding_index++)
			{
				SpvReflectDescriptorBinding* spirv_binding = descriptor_set->bindings[binding_index];

				VkDescriptorSetLayoutBinding set_binding = {};
				set_binding.stageFlags = (VkShaderStageFlagBits)module.shader_stage;
				set_binding.binding = spirv_binding->binding;
				set_binding.descriptorType = (VkDescriptorType)spirv_binding->descriptor_type;
				set_binding.descriptorCount = 1;
				for (size_t array_index = 0; array_index < spirv_binding->array.dims_count; array_index++)
				{
					set_binding.descriptorCount *= spirv_binding->array.dims[array_index];
				}
				binding_layouts.push_back(set_binding);
			}

			VkDescriptorSetLayout descriptor_set_layout;
			VkDescriptorSetLayoutCreateInfo layout_info = {};
			layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layout_info.bindingCount = (uint32_t)binding_layouts.size();
			layout_info.pBindings = binding_layouts.data();

			if (vkCreateDescriptorSetLayout(this->device, &layout_info, nullptr, &descriptor_set_layout) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create descriptor set layout!");
			}

			this->descriptor_set_layouts.push_back(descriptor_set_layout);
		}}

	//Push Constants
	{
		uint32_t count = 0;
		spvReflectEnumeratePushConstantBlocks(&module, &count, NULL);
		vector<SpvReflectBlockVariable*> push_constants(count);
		spvReflectEnumeratePushConstantBlocks(&module, &count, push_constants.data());

		for (size_t block_index = 0; block_index < push_constants.size(); block_index++)
		{
			VkPushConstantRange push_constant = {};
			push_constant.stageFlags = (VkShaderStageFlagBits)module.shader_stage;
			push_constant.offset = push_constants[block_index]->offset;
			push_constant.size = push_constants[block_index]->size;

			this->push_constant_layouts.push_back(push_constant);
		}
	}

	spvReflectDestroyShaderModule(&module);
}

VulkanShader::VulkanShader(VkDevice device, string vert_data, string frag_data)
{
	this->device = device;

	this->vertShaderModule = createShaderModule(vert_data);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = this->vertShaderModule;
	vertShaderStageInfo.pName = "main";

	this->shaderStages.push_back(vertShaderStageInfo);

	if (frag_data != "")
	{
		this->fragShaderModule = createShaderModule(frag_data);

		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = this->fragShaderModule;
		fragShaderStageInfo.pName = "main";

		this->shaderStages.push_back(fragShaderStageInfo);
	}


	VkPipelineLayoutCreateInfo pipeline_layout_info = {};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	pipeline_layout_info.setLayoutCount = (uint32_t)this->descriptor_set_layouts.size();
	pipeline_layout_info.pSetLayouts = this->descriptor_set_layouts.data();

	pipeline_layout_info.pushConstantRangeCount = (uint32_t)this->push_constant_layouts.size();
	pipeline_layout_info.pPushConstantRanges = this->push_constant_layouts.data();

	if (vkCreatePipelineLayout(this->device, &pipeline_layout_info, nullptr, &this->pipeline_layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

VulkanShader::~VulkanShader()
{
	vkDestroyShaderModule(this->device, this->vertShaderModule, nullptr);

	if (this->fragShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(this->device, this->fragShaderModule, nullptr);
	}
}

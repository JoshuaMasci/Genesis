#include "VulkanShader.hpp"

#include "spirv_reflect.h"

using namespace Genesis;

VulkanShaderModule::VulkanShaderModule(VkDevice device, string shader_data)
{
	this->device = device;

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shader_data.length();
	createInfo.pCode = (uint32_t*)shader_data.data();

	if (vkCreateShaderModule(this->device, &createInfo, nullptr, &this->shader_module) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}

	SpvReflectShaderModule module;
	SpvReflectResult result = spvReflectCreateShaderModule(shader_data.length(), (uint32_t*)shader_data.data(), &module);

	this->shader_stage = (VkShaderStageFlagBits)module.shader_stage;

	uint32_t push_constant_count = 0;
	spvReflectEnumeratePushConstantBlocks(&module, &push_constant_count, NULL);
	if (push_constant_count > 0)
	{
		//Push Constants not supported
		throw std::runtime_error("push constants are supported!");
	}

	uint32_t descriptor_sets_count = 0;
	spvReflectEnumerateDescriptorSets(&module, &descriptor_sets_count, NULL);
	if (descriptor_sets_count > 1)
	{
		//Push Constants not supported
		throw std::runtime_error("more than one DescriptorSet is not supported");
	}

	if (descriptor_sets_count > 0)
	{
		SpvReflectDescriptorSet* descriptor_set_spv;
		spvReflectEnumerateDescriptorSets(&module, &descriptor_sets_count, &descriptor_set_spv);

		this->bindings = Array<VkDescriptorSetLayoutBinding>(descriptor_set_spv->binding_count);
		for (size_t binding_index = 0; binding_index < descriptor_set_spv->binding_count; binding_index++)
		{
			this->bindings[binding_index].binding = descriptor_set_spv->bindings[binding_index]->binding;
			this->bindings[binding_index].descriptorType = (VkDescriptorType)descriptor_set_spv->bindings[binding_index]->descriptor_type;
			this->bindings[binding_index].descriptorCount = 1;
			for (uint32_t i_dim = 0; i_dim < descriptor_set_spv->bindings[binding_index]->array.dims_count; ++i_dim)
			{
				this->bindings[binding_index].descriptorCount *= descriptor_set_spv->bindings[binding_index]->array.dims[i_dim];
			}

			this->bindings[binding_index].stageFlags = this->shader_stage;

			this->bindings[binding_index].pImmutableSamplers = nullptr;
		}
	}

	spvReflectDestroyShaderModule(&module);
}

VulkanShaderModule::~VulkanShaderModule()
{
	vkDestroyShaderModule(this->device, this->shader_module, nullptr);
}

VkPipelineShaderStageCreateInfo VulkanShaderModule::getStageInfo()
{
	VkPipelineShaderStageCreateInfo shader_stage_info = {};
	shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_info.stage = this->shader_stage;
	shader_stage_info.module = this->shader_module;
	shader_stage_info.pName = "main";

	return shader_stage_info;
}

VulkanShader::VulkanShader(VkDevice device, string vert_data, string frag_data)
{
	this->device = device;

	this->vert_module = new VulkanShaderModule(this->device, vert_data);

	if (frag_data != "")
	{
		this->frag_module = new VulkanShaderModule(this->device, frag_data);
	}

	size_t vert_binding_count = this->vert_module->bindings.size();
	size_t binding_count = vert_binding_count + ((this->frag_module != nullptr) ? this->frag_module->bindings.size() : 0);

	if (binding_count > 0)
	{
		Array<VkDescriptorSetLayoutBinding> bindings(binding_count);

		for (size_t i = 0; i < vert_binding_count; i++)
		{
			bindings[i] = this->vert_module->bindings[i];
		}

		if (this->frag_module != nullptr)
		{
			for (size_t i = 0; i < this->frag_module->bindings.size(); i++)
			{
				bindings[i + vert_binding_count] = this->frag_module->bindings[i];
			}
		}

		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info;
		descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_set_layout_info.pNext = NULL;
		descriptor_set_layout_info.flags = 0;
		descriptor_set_layout_info.bindingCount = (uint32_t)bindings.size();
		descriptor_set_layout_info.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(this->device, &descriptor_set_layout_info, nullptr, &this->descriptor_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	VkPipelineLayoutCreateInfo pipeline_layout_info = {};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.pushConstantRangeCount = 0;
	pipeline_layout_info.setLayoutCount = ((this->descriptor_layout != VK_NULL_HANDLE) ? 1 : 0);
	pipeline_layout_info.pSetLayouts = &this->descriptor_layout;

	if (vkCreatePipelineLayout(this->device, &pipeline_layout_info, nullptr, &this->pipeline_layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	this->shader_stages.push_back(this->vert_module->getStageInfo());

	if (this->frag_module != nullptr)
	{
		this->shader_stages.push_back(this->frag_module->getStageInfo());
	}
}

VulkanShader::~VulkanShader()
{
	vkDestroyDescriptorSetLayout(this->device, this->descriptor_layout, nullptr);
	vkDestroyPipelineLayout(this->device, this->pipeline_layout, nullptr);

	delete this->vert_module;

	if (this->frag_module != nullptr)
	{
		delete this->frag_module;
	}
}

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

	return shaderModule;
}

void VulkanShader::extractLayout(string shader_code, VulkanDescriptorSetLayouts* layouts)
{
	SpvReflectShaderModule module;
	SpvReflectResult result = spvReflectCreateShaderModule(shader_code.length(), (uint32_t*)shader_code.data(), &module);

	VkShaderStageFlags shader_stage = (VkShaderStageFlagBits)module.shader_stage;

	//Descriptor Sets
	{
		uint32_t count = 0;
		spvReflectEnumerateDescriptorSets(&module, &count, NULL);
		vector<SpvReflectDescriptorSet*> descriptor_sets(count);
		spvReflectEnumerateDescriptorSets(&module, &count, descriptor_sets.data());
		for (size_t descriptor_set_index = 0; descriptor_set_index < descriptor_sets.size(); descriptor_set_index++)
		{
			SpvReflectDescriptorSet* descriptor_set = descriptor_sets[descriptor_set_index];

			if (descriptor_set->binding_count > 1)
			{
				//Only one binding supported for each set
			}

			string set_name = descriptor_set->bindings[0]->name;

			if (this->descriptor_map.find(set_name) != this->descriptor_map.end())
			{
				//Already exists
				continue;
			}

			DescriptorSetInfo& info = this->descriptor_map[set_name];

			info.index = descriptor_set->bindings[0]->set;
			info.type = (VkDescriptorType)descriptor_set->bindings[0]->descriptor_type;
			info.size = descriptor_set->bindings[0]->block.size;
			info.shader_stage = shader_stage;


			VkDescriptorSetLayout descriptor_set_layout = layouts->getDescriptorSetLayout(info.type, shader_stage);
			this->descriptor_set_layouts.push_back(descriptor_set_layout);
		}
	}

	//Push Constants
	{
		uint32_t push_constant_count = 0;
		spvReflectEnumeratePushConstantBlocks(&module, &push_constant_count, NULL);
		if (push_constant_count > 0)
		{
			//Push Constants not supported
		}
	}

	spvReflectDestroyShaderModule(&module);
}

VulkanShader::VulkanShader(VkDevice device, string vert_data, string frag_data, VulkanDescriptorSetLayouts* layouts)
{
	size_t sz = sizeof(VkExtent2D);


	this->device = device;

	this->vertShaderModule = createShaderModule(vert_data);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = this->vertShaderModule;
	vertShaderStageInfo.pName = "main";

	this->shader_stages.push_back(vertShaderStageInfo);

	if (frag_data != "")
	{
		this->fragShaderModule = createShaderModule(frag_data);

		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = this->fragShaderModule;
		fragShaderStageInfo.pName = "main";

		this->shader_stages.push_back(fragShaderStageInfo);
	}

	//Pipeline Layout Creation
	this->extractLayout(vert_data, layouts);

	if (frag_data != "")
	{
		this->extractLayout(frag_data, layouts);
	}

	VkPipelineLayoutCreateInfo pipeline_layout_info = {};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	pipeline_layout_info.setLayoutCount = (uint32_t)this->descriptor_set_layouts.size();
	pipeline_layout_info.pSetLayouts = this->descriptor_set_layouts.data();

	pipeline_layout_info.pushConstantRangeCount = 0;

	if (vkCreatePipelineLayout(this->device, &pipeline_layout_info, nullptr, &this->pipeline_layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

VulkanShader::~VulkanShader()
{
	vkDestroyPipelineLayout(this->device, this->pipeline_layout, nullptr);

	vkDestroyShaderModule(this->device, this->vertShaderModule, nullptr);

	if (this->fragShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(this->device, this->fragShaderModule, nullptr);
	}
}

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

vector<VulkanShader::DescriptorIndex> VulkanShader::extractLayout(string shader_code, VulkanDescriptorSetLayouts* layouts)
{
	SpvReflectShaderModule module;
	SpvReflectResult result = spvReflectCreateShaderModule(shader_code.length(), (uint32_t*)shader_code.data(), &module);

	VkShaderStageFlags shader_stage = (VkShaderStageFlagBits)module.shader_stage;

	//Push Constants
	{
		uint32_t push_constant_count = 0;
		spvReflectEnumeratePushConstantBlocks(&module, &push_constant_count, NULL);
		if (push_constant_count > 0)
		{
			//Push Constants not supported
		}
	}

	vector<VulkanShader::DescriptorIndex> descriptors;

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

			//TODO merge duplicates in diffrent stages
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

			VkDescriptorSetLayout descriptor_set_layout = layouts->getDescriptorSetLayout(info.type);
			descriptors.push_back({info.index, descriptor_set_layout});
		}
	}

	spvReflectDestroyShaderModule(&module);

	return descriptors;
}

VulkanShader::VulkanShader(VkDevice device, string vert_data, string frag_data, VulkanDescriptorSetLayouts* layouts)
{
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

	vector<VulkanShader::DescriptorIndex> vert_descriptors;
	vector<VulkanShader::DescriptorIndex> frag_descriptors;

	//Pipeline Layout Creation
	vert_descriptors = this->extractLayout(vert_data, layouts);

	if (frag_data != "")
	{
		frag_descriptors = this->extractLayout(frag_data, layouts);
	}

	Array<VkDescriptorSetLayout> descriptor_set_layouts(vert_descriptors.size() + frag_descriptors.size());
	for (int i = 0; i < vert_descriptors.size(); i++)
	{
		VulkanShader::DescriptorIndex descriptor = vert_descriptors[i];
		descriptor_set_layouts[descriptor.index] = descriptor.layout;
	}

	for (int i = 0; i < frag_descriptors.size(); i++)
	{
		VulkanShader::DescriptorIndex descriptor = frag_descriptors[i];
		descriptor_set_layouts[descriptor.index] = descriptor.layout;
	}

	VkPipelineLayoutCreateInfo pipeline_layout_info = {};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.pushConstantRangeCount = 0;
	pipeline_layout_info.setLayoutCount = (uint32_t)descriptor_set_layouts.size();
	pipeline_layout_info.pSetLayouts = descriptor_set_layouts.data();

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

bool Genesis::VulkanShader::hasDescriptorSetInfo(string uniform_name)
{
	if (this->descriptor_map.find(uniform_name) != this->descriptor_map.end())
	{
		return true;
	}

	return false;
}

VulkanShader::DescriptorSetInfo VulkanShader::getDescriptorSetInfo(string uniform_name)
{
	return this->descriptor_map[uniform_name];
}

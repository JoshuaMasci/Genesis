#include "VulkanShader.hpp"

#include "spirv_reflect.h"

using namespace Genesis;

VulkanShaderModule::VulkanShaderModule(VkDevice device, string& shader_data)
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
		List<SpvReflectBlockVariable*> push_blocks(push_constant_count);
		this->push_constant.resize(push_constant_count);
		spvReflectEnumeratePushConstantBlocks(&module, &push_constant_count, push_blocks.data());

		for (size_t i = 0; i < push_blocks.size(); i++)
		{
			this->push_constant[i].size = push_blocks[i]->padded_size;
			this->push_constant[i].offset = push_blocks[i]->absolute_offset;
			this->push_constant[i].stageFlags = VK_SHADER_STAGE_ALL; //TODO use specific stage flags
		}
	}

	uint32_t descriptor_sets_count = 0;
	spvReflectEnumerateDescriptorSets(&module, &descriptor_sets_count, NULL);

	List<SpvReflectDescriptorSet*> descriptor_sets_spv(descriptor_sets_count);
	spvReflectEnumerateDescriptorSets(&module, &descriptor_sets_count, descriptor_sets_spv.data());

	for (size_t i = 0; i < descriptor_sets_spv.size(); i++)
	{
		uint32_t descriptor_index = descriptor_sets_spv[i]->set;

		if (descriptor_index >= this->descriptor_sets.size())
		{
			this->descriptor_sets.resize(descriptor_index + 1);
		}

		for (size_t j = 0; j < descriptor_sets_spv[i]->binding_count; j++)
		{
			uint32_t binding_index = descriptor_sets_spv[i]->bindings[j]->binding;

			if (binding_index >= this->descriptor_sets[descriptor_index].size())
			{
				this->descriptor_sets[descriptor_index].resize(binding_index + 1);
			}
		}
	}

	for (size_t i = 0; i < descriptor_sets_spv.size(); i++)
	{
		uint32_t descriptor_index = descriptor_sets_spv[i]->set;
		for (size_t j = 0; j < descriptor_sets_spv[i]->binding_count; j++)
		{
			uint32_t binding_index = descriptor_sets_spv[i]->bindings[j]->binding;

			this->descriptor_sets[descriptor_index][binding_index].type = (VkDescriptorType)descriptor_sets_spv[i]->bindings[j]->descriptor_type;

			uint32_t count = 1;
			for (uint32_t i_dim = 0; i_dim < descriptor_sets_spv[i]->bindings[j]->array.dims_count; ++i_dim)
			{
				count *= descriptor_sets_spv[i]->bindings[j]->array.dims[i_dim];
			}
			this->descriptor_sets[descriptor_index][binding_index].count = count;
			this->descriptor_sets[descriptor_index][binding_index].size = descriptor_sets_spv[i]->bindings[j]->block.padded_size;
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

void fillDescriptorSetLayouts(List<List<VkDescriptorSetLayoutBinding>>& destination, List<List<DescriptorSetBindingModule>>& source, VkShaderStageFlags stage)
{
	if (destination.size() < source.size())
	{
		destination.resize(source.size());
	}

	for (int i = 0; i < destination.size(); i++)
	{
		if (destination[i].size() < source[i].size())
		{
			destination[i].resize(source[i].size());
			for (int j = 0; j < destination[i].size(); j++)
			{
				//Default Values
				destination[i][j].binding = j;
				destination[i][j].descriptorCount = 0;
				destination[i][j].pImmutableSamplers = nullptr;
				destination[i][j].stageFlags = 0;
			}
		}

		for (int j = 0; j < destination[i].size(); j++)
		{
			if (j >= source[i].size())
			{
				break;
			}

			if (source[i][j].count > 0)
			{
				if (destination[i][j].descriptorCount > 0)
				{
					if ((destination[i][j].descriptorType != source[i][j].type) || (destination[i][j].descriptorCount != source[i][j].count))
					{
						throw std::runtime_error("Shader type mismatch");
					}

					destination[i][j].stageFlags |= stage;
				}
				else
				{
					destination[i][j].descriptorType = source[i][j].type;
					destination[i][j].descriptorCount = source[i][j].count;
					destination[i][j].stageFlags = stage;
				}
			}
		}
	}
}

void fillDescriptorSetBindings(List<List<DescriptorSetBinding>>& destination, List<List<DescriptorSetBindingModule>>& source, VkShaderStageFlags stage)
{
	if (source.size() <= 0)
	{
		return;
	}

	if (destination.size() < source.size())
	{
		destination.resize(source.size());
	}

	for (int i = 0; i < destination.size(); i++)
	{
		if (destination[i].size() < source[i].size())
		{
			destination[i].resize(source[i].size());
		}

		for (int j = 0; j < destination[i].size(); j++)
		{
			if (j >= source[i].size())
			{
				break;
			}

			if (source[i][j].count > 0)
			{
				if (destination[i][j].count > 0)
				{
					if ((destination[i][j].type != source[i][j].type) || (destination[i][j].count != source[i][j].count))
					{
						throw std::runtime_error("Shader type mismatch");
					}

					destination[i][j].stage |= stage;
				}
				else
				{
					destination[i][j].type = source[i][j].type;
					destination[i][j].count = source[i][j].count;
					destination[i][j].stage = stage;
				}
			}
		}
	}
}

VulkanShader::VulkanShader(VkDevice device, string& vert_data, string& frag_data, VulkanLayoutPool* layout_pool)
{
	this->device = device;

	this->vert_module = new VulkanShaderModule(this->device, vert_data);
	fillDescriptorSetBindings(this->descriptor_set_bindings, this->vert_module->descriptor_sets, this->vert_module->shader_stage);

	size_t vert_push_constant_count = this->vert_module->push_constant.size();
	List<VkPushConstantRange> push_constant(vert_push_constant_count);
	for (size_t i = 0; i < vert_push_constant_count; i++)
	{
		push_constant[i] = this->vert_module->push_constant[i];
	}

	if (frag_data != "")
	{
		this->frag_module = new VulkanShaderModule(this->device, frag_data);
		fillDescriptorSetBindings(this->descriptor_set_bindings, this->frag_module->descriptor_sets, this->frag_module->shader_stage);

		size_t frag_push_constant_count = this->frag_module->push_constant.size();
		push_constant.resize(vert_push_constant_count + frag_push_constant_count);
		for (size_t i = 0; i < frag_push_constant_count; i++)
		{
			push_constant[vert_push_constant_count + i] = this->frag_module->push_constant[i];
		}
	}

	List<List<VkDescriptorSetLayoutBinding>> descriptor_sets(this->descriptor_set_bindings.size());
	for (size_t set = 0; set < this->descriptor_set_bindings.size(); set++)
	{
		descriptor_sets[set].resize(this->descriptor_set_bindings[set].size());
		for (size_t binding = 0; binding < this->descriptor_set_bindings[set].size(); binding++)
		{
			descriptor_sets[set][binding].binding = (uint32_t)binding;
			descriptor_sets[set][binding].descriptorType = this->descriptor_set_bindings[set][binding].type;
			descriptor_sets[set][binding].descriptorCount = this->descriptor_set_bindings[set][binding].count;
			descriptor_sets[set][binding].stageFlags = this->descriptor_set_bindings[set][binding].stage;
			descriptor_sets[set][binding].pImmutableSamplers = nullptr;
		}
	}

	this->descriptor_layouts.resize(descriptor_sets.size());
	for (size_t i = 0; i < this->descriptor_layouts.size(); i++)
	{
		this->descriptor_layouts[i] = layout_pool->getDescriptorLayout(descriptor_sets[i]);
	}
	this->pipeline_layout = layout_pool->getPipelineLayout(this->descriptor_layouts, push_constant);

	this->shader_stages.push_back(this->vert_module->getStageInfo());

	if (this->frag_module != nullptr)
	{
		this->shader_stages.push_back(this->frag_module->getStageInfo());
	}
}

VulkanShader::~VulkanShader()
{
	delete this->vert_module;

	if (this->frag_module != nullptr)
	{
		delete this->frag_module;
	}
}
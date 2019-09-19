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

	//TEMP
	if (descriptor_sets_count > 1)
	{
		//Push Constants not supported
		throw std::runtime_error("more than one DescriptorSet is not supported");
	}

	Array<SpvReflectDescriptorSet*> descriptor_sets_spv(descriptor_sets_count);
	spvReflectEnumerateDescriptorSets(&module, &descriptor_sets_count, descriptor_sets_spv.data());

	//TODO
	/*for (size_t i = 0; i < descriptor_sets_spv.size(); i++)
	{

	}*/

	if (descriptor_sets_count > 0)
	{
		SpvReflectDescriptorSet* descriptor_set_spv = descriptor_sets_spv[0];

		this->shader_bindings = Array<ShaderBinding>(descriptor_set_spv->binding_count);
		for (size_t binding_index = 0; binding_index < this->shader_bindings.size(); binding_index++)
		{
			this->shader_bindings[binding_index] = {};
			ShaderBinding* shader_binding = &this->shader_bindings[binding_index];
			shader_binding->name = descriptor_set_spv->bindings[binding_index]->name;
			shader_binding->binding_location = descriptor_set_spv->bindings[binding_index]->binding;
			shader_binding->type = (VkDescriptorType)descriptor_set_spv->bindings[binding_index]->descriptor_type;
			shader_binding->shader_stage = this->shader_stage;

			shader_binding->total_size = descriptor_set_spv->bindings[binding_index]->block.padded_size;
			shader_binding->variables = Array<ShaderBindingVariable>(descriptor_set_spv->bindings[binding_index]->block.member_count);
			for (uint32_t var_index = 0; var_index < shader_binding->variables.size(); var_index++)
			{
				SpvReflectBlockVariable var = descriptor_set_spv->bindings[binding_index]->block.members[var_index];
				shader_binding->variables[var_index].name = var.name;
				shader_binding->variables[var_index].offset = var.absolute_offset;
				shader_binding->variables[var_index].size = var.size;
				shader_binding->variables[var_index].padded_size = var.padded_size;
			}

			shader_binding->array_count = 1;
			for (uint32_t i_dim = 0; i_dim < descriptor_set_spv->bindings[binding_index]->array.dims_count; ++i_dim)
			{
				shader_binding->array_count *= descriptor_set_spv->bindings[binding_index]->array.dims[i_dim];
			}
		}
	}

	/*		printf("Binding %d: %s , %d = %d\n", descriptor_set_spv->bindings[binding_index]->set, descriptor_set_spv->bindings[binding_index]->name, descriptor_set_spv->bindings[binding_index]->block.size, descriptor_set_spv->bindings[binding_index]->block.padded_size);
			uint32_t member_count = descriptor_set_spv->bindings[binding_index]->block.member_count;
			for (uint32_t i = 0; i < member_count; i++)
			{
				SpvReflectBlockVariable var = descriptor_set_spv->bindings[binding_index]->block.members[i];
				printf("    %d: %s, %d = %d\n", var.absolute_offset, var.name, var.size, var.padded_size);
			}
			printf("\n");

			this->bindings[binding_index].binding = descriptor_set_spv->bindings[binding_index]->binding;
			this->bindings[binding_index].descriptorType = (VkDescriptorType)descriptor_set_spv->bindings[binding_index]->descriptor_type;
			this->bindings[binding_index].descriptorCount = 1;
			for (uint32_t i_dim = 0; i_dim < descriptor_set_spv->bindings[binding_index]->array.dims_count; ++i_dim)
			{
				this->bindings[binding_index].descriptorCount *= descriptor_set_spv->bindings[binding_index]->array.dims[i_dim];
			}

			this->bindings[binding_index].stageFlags = this->shader_stage;

			this->bindings[binding_index].pImmutableSamplers = nullptr;*/

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

VkDescriptorSetLayoutBinding toVkBinding(ShaderBinding shader_binding)
{
	VkDescriptorSetLayoutBinding binding = {};
	binding.binding = shader_binding.binding_location;
	
	binding.descriptorType = shader_binding.type;
	/*if (binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
	{
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	}*/

	binding.stageFlags = shader_binding.shader_stage;
	binding.descriptorCount = shader_binding.array_count;
	binding.pImmutableSamplers = nullptr;
	return binding;
}

VulkanShader::VulkanShader(VkDevice device, string vert_data, string frag_data)
{
	this->device = device;

	this->vert_module = new VulkanShaderModule(this->device, vert_data);

	if (frag_data != "")
	{
		this->frag_module = new VulkanShaderModule(this->device, frag_data);
	}

	size_t vert_binding_count = this->vert_module->shader_bindings.size();
	size_t binding_count = vert_binding_count + ((this->frag_module != nullptr) ? this->frag_module->shader_bindings.size() : 0);

	if (binding_count > 0)
	{
		Array<VkDescriptorSetLayoutBinding> bindings(binding_count);

		for (size_t i = 0; i < vert_binding_count; i++)
		{
			bindings[i] = toVkBinding(this->vert_module->shader_bindings[i]);
			
			if (!has_value(this->name_bindings, this->vert_module->shader_bindings[i].name))
			{
				this->name_bindings[this->vert_module->shader_bindings[i].name] = &this->vert_module->shader_bindings[i];
			}
			else
			{
				throw std::runtime_error("Duplicate Uniform Name!!!!");
			}
		}

		if (this->frag_module != nullptr)
		{
			for (size_t i = 0; i < this->frag_module->shader_bindings.size(); i++)
			{
				bindings[i + vert_binding_count] = toVkBinding(this->frag_module->shader_bindings[i]);

				if (!has_value(this->name_bindings, this->frag_module->shader_bindings[i].name))
				{
					this->name_bindings[this->frag_module->shader_bindings[i].name] = &this->frag_module->shader_bindings[i];
				}
				else
				{
					throw std::runtime_error("Duplicate Uniform Name!!!!");
				}
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

ShaderBindingVariableLocation VulkanShader::getVariableLocation(string name)
{
	if (has_value(this->name_location_cache, name))
	{
		return this->name_location_cache[name];
	}

	ShaderBindingVariableLocation new_loc;

	size_t find_loc = name.find_first_of('.');
	Genesis::string binding_name = name.substr(0, find_loc);
	Genesis::string var_name = name.substr(find_loc + 1, (name.size() - (find_loc + 1)));

	if (!has_value(this->name_bindings, binding_name))
	{
		throw std::runtime_error("Uniform doesn't exsit");
	}

	ShaderBinding* binding = this->name_bindings[binding_name];
	new_loc.binding = binding->binding_location;

	if (binding->type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
	{
		new_loc.variable_size = 0;
		new_loc.variable_offset = 0;
	}
	else
	{
		size_t var_index = 0;
		bool found = false;
		for (; var_index < binding->variables.size(); var_index++)
		{
			if (var_name == binding->variables[var_index].name)
			{
				found = true;
				break; //Found it
			}
		}

		if (found != true)
		{
			throw std::runtime_error("Uniform Variable doesn't exsit");
		}

		new_loc.variable_size = binding->variables[var_index].size;
		new_loc.variable_offset = binding->variables[var_index].offset;
	}

	this->name_location_cache[name] = new_loc;
	return new_loc;
}

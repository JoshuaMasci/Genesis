#include "VulkanShader.hpp"

#include "spirv_reflect.h"

using namespace Genesis;

void addBindingVariable(map<string, ShaderVariableLocation>* variable_loactions, uint32_t binding_index, string block_name, SpvReflectBlockVariable* block)
{
	string var_name = block_name;
	ShaderVariableLocation var_info;
	var_info.variable_offset = block->absolute_offset;
	var_info.variable_size = block->size;
	var_info.type = ShaderVariableType::Binding;
	var_info.location.binding_index = binding_index;
	variable_loactions->emplace(var_name, var_info);

	for (size_t i = 0; i < block->member_count; i++)
	{
		addBindingVariable(variable_loactions, binding_index, var_name + "." + block->members[i].name, &block->members[i]);
	}
};

void addConstVariable(map<string, ShaderVariableLocation>* variable_loactions, VkShaderStageFlagBits variable_stage, string block_name, SpvReflectBlockVariable* block)
{
	string var_name = block_name;
	ShaderVariableLocation var_info;
	var_info.variable_offset = block->absolute_offset;
	var_info.variable_size = block->size;
	var_info.type = ShaderVariableType::PushConstant;
	var_info.location.variable_stage = variable_stage;
	variable_loactions->emplace(var_name, var_info);

	for (size_t i = 0; i < block->member_count; i++)
	{
		addConstVariable(variable_loactions, variable_stage, var_name + "." + block->members[i].name, &block->members[i]);
	}
};

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
	if (push_constant_count == 0)
	{
		this->push_constant.name = "";
		this->push_constant.total_size = 0;
	}
	else if (push_constant_count == 1)
	{
		SpvReflectBlockVariable* push_block;

		spvReflectEnumeratePushConstantBlocks(&module, &push_constant_count, &push_block);
		this->push_constant.name = push_block->var_name;
		this->push_constant.shader_stage = this->shader_stage;
		this->push_constant.total_size = push_block->size;
		this->push_constant.variables = push_block->padded_size;

		addConstVariable(&this->variable_loactions, this->shader_stage, push_block->var_name, push_block);
	}
	else if (push_constant_count > 1)
	{
		//Push Constants not supported
		throw std::runtime_error("more than one PushConstant is not supported");
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
		
			addBindingVariable(&this->variable_loactions, descriptor_set_spv->bindings[binding_index]->binding, descriptor_set_spv->bindings[binding_index]->name, &descriptor_set_spv->bindings[binding_index]->block);
		}
	}

	spvReflectDestroyShaderModule(&module);
}

VulkanShaderModule::~VulkanShaderModule()
{
	vkDestroyShaderModule(this->device, this->shader_module, nullptr);
}

void VulkanShaderModule::addVariables(map<string, ShaderVariableLocation>& variable_loactions)
{
	variable_loactions.insert(this->variable_loactions.begin(), this->variable_loactions.end());
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
	binding.descriptorCount = 1;//shader_binding.array_count;
	binding.pImmutableSamplers = nullptr;
	return binding;
}

VulkanShader::VulkanShader(VkDevice device, string& vert_data, string& frag_data)
{
	this->device = device;

	this->vert_module = new VulkanShaderModule(this->device, vert_data);

	if (frag_data != "")
	{
		this->frag_module = new VulkanShaderModule(this->device, frag_data);
	}

	this->vert_module->addVariables(this->name_variable_loaction_cache);
	if (this->vert_module->push_constant.total_size != 0)
	{
		this->name_constants[this->vert_module->push_constant.name] = &this->vert_module->push_constant;
	}

	if (this->frag_module != nullptr)
	{
		this->frag_module->addVariables(this->name_variable_loaction_cache);
		if (this->frag_module->push_constant.total_size != 0)
		{
			this->name_constants[this->frag_module->push_constant.name] = &this->frag_module->push_constant;
		}
	}

	size_t vert_binding_count = this->vert_module->shader_bindings.size();
	size_t binding_count = vert_binding_count + ((this->frag_module != nullptr) ? this->frag_module->shader_bindings.size() : 0);

	if (binding_count > 0)
	{
		Array<VkDescriptorSetLayoutBinding> bindings(binding_count);
		for (size_t i = 0; i < vert_binding_count; i++)
		{
			bindings[i] = toVkBinding(this->vert_module->shader_bindings[i]);
			string binding_name = this->vert_module->shader_bindings[i].name;
			if (!has_value(this->name_bindings, binding_name))
			{
				this->name_bindings[binding_name] = &this->vert_module->shader_bindings[i];
			}
			else
			{
				throw std::runtime_error("Duplicate Uniform Name!!!!");
			}
		}

		if (this->frag_module != nullptr)
		{
			this->frag_module->addVariables(this->name_variable_loaction_cache);

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


	Array<VkPushConstantRange> push_constants(this->name_constants.size());
	size_t i = 0;
	for (auto constant : this->name_constants)
	{
		push_constants[i].offset = 0;
		push_constants[i].size = constant.second->total_size;
		push_constants[i].stageFlags = constant.second->shader_stage;
		i++;
	}
	pipeline_layout_info.pushConstantRangeCount = (uint32_t)push_constants.size();
	pipeline_layout_info.pPushConstantRanges = push_constants.data();

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

ShaderVariableLocation Genesis::VulkanShader::getVariableLocation(string name)
{
	if (!has_value(this->name_variable_loaction_cache, name))
	{
		throw std::runtime_error("Binding Variable doesn't exsit");
	}

	return this->name_variable_loaction_cache[name];
}
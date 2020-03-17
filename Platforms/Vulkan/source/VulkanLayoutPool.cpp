#include "VulkanLayoutPool.hpp"

#include "Genesis/Debug/Assert.hpp"

#include "Genesis/Core/MurmurHash2.hpp"

#include <stdexcept>

using namespace Genesis;

VulkanLayoutPool::VulkanLayoutPool(VkDevice device)
{
	this->device = device;
}

VulkanLayoutPool::~VulkanLayoutPool()
{
	for (auto layout : this->descriptor_layouts)
	{
		vkDestroyDescriptorSetLayout(this->device, layout.second, nullptr);
	}

	for (auto layout : this->pipeline_layouts)
	{
		vkDestroyPipelineLayout(this->device, layout.second, nullptr);
	}
}

VkDescriptorSetLayout VulkanLayoutPool::getDescriptorLayout(List<VkDescriptorSetLayoutBinding>& bindings)
{
	MurmurHash2 binding_hash;
	binding_hash.addData((const uint8_t*)bindings.data(), (uint32_t)(bindings.size() * sizeof(VkDescriptorSetLayoutBinding)));
	uint32_t hash_value = binding_hash.end();

	VkDescriptorSetLayout layout = VK_NULL_HANDLE;
	this->descriptor_map_lock.lock();
	if (has_value(this->descriptor_layouts, hash_value))
	{
		layout = this->descriptor_layouts[hash_value];
	}
	else
	{
		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info;
		descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_set_layout_info.pNext = NULL;
		descriptor_set_layout_info.flags = 0;
		descriptor_set_layout_info.bindingCount = (uint32_t)bindings.size();
		descriptor_set_layout_info.pBindings = bindings.data();
		GENESIS_ENGINE_ASSERT_ERROR((vkCreateDescriptorSetLayout(this->device, &descriptor_set_layout_info, nullptr, &layout) == VK_SUCCESS), "failed to create descriptor set layout");

		this->descriptor_layouts[hash_value] = layout;
	}
	this->descriptor_map_lock.unlock();
	return layout;
}

VkDescriptorType getDescriptorType(BindingType type)
{
	switch (type)
	{
	case BindingType::Sampler:
		return VK_DESCRIPTOR_TYPE_SAMPLER;
	case BindingType::Sampled_Image:
		return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	case BindingType::Storage_Image:
		return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	case BindingType::Uniform_Buffer:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	case BindingType::Storage_Buffer:
		return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
	}

	//Error
	return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

VkShaderStageFlags getShaderStageFlags(ShaderStage stage)
{
	VkShaderStageFlags shader_stage = 0;
	if (stage && (ShaderStage)ShaderStageBits::VERTEX)
	{
		shader_stage |= VK_SHADER_STAGE_VERTEX_BIT;
	}
	if (stage && (ShaderStage)ShaderStageBits::FRAGMENT)
	{
		shader_stage |= VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	if (stage && (ShaderStage)ShaderStageBits::GEOMETRY)
	{
		shader_stage |= VK_SHADER_STAGE_GEOMETRY_BIT;
	}
	if (stage && (ShaderStage)ShaderStageBits::COMPUTE)
	{
		shader_stage |= VK_SHADER_STAGE_COMPUTE_BIT;
	}
	return shader_stage;
}

VkDescriptorSetLayout VulkanLayoutPool::getDescriptorLayout(const DescriptorSetLayoutCreateInfo& create_info)
{
	MurmurHash2 binding_hash;
	binding_hash.addData((const uint8_t*)create_info.descriptor_bindings, (uint32_t)(create_info.descriptor_bindings_count * sizeof(DescriptorBinding)));
	uint32_t hash_value = binding_hash.end();

	VkDescriptorSetLayout layout = VK_NULL_HANDLE;
	this->descriptor_map_lock.lock();
	if (has_value(this->descriptor_layouts, hash_value))
	{
		layout = this->descriptor_layouts[hash_value];
	}
	else
	{
		List<VkDescriptorSetLayoutBinding> bindings(create_info.descriptor_bindings_count);
		for (uint32_t i = 0; i < create_info.descriptor_bindings_count; i++)
		{
			bindings[i] = {};
			bindings[i].binding = i;
			bindings[i].descriptorType = getDescriptorType(create_info.descriptor_bindings[i].binding_type);
			bindings[i].descriptorCount = 1;
			bindings[i].stageFlags = getShaderStageFlags(create_info.descriptor_bindings[i].binding_stages);
			bindings[i].pImmutableSamplers = nullptr;
		}

		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info;
		descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_set_layout_info.pNext = NULL;
		descriptor_set_layout_info.flags = 0;
		descriptor_set_layout_info.bindingCount = (uint32_t)bindings.size();
		descriptor_set_layout_info.pBindings = bindings.data();
		GENESIS_ENGINE_ASSERT_ERROR((vkCreateDescriptorSetLayout(this->device, &descriptor_set_layout_info, nullptr, &layout) == VK_SUCCESS), "failed to create descriptor set layout");

		this->descriptor_layouts[hash_value] = layout;
	}
	this->descriptor_map_lock.unlock();
	return layout;
}

VkPipelineLayout VulkanLayoutPool::getPipelineLayout(List<VkDescriptorSetLayout>& layouts, List<VkPushConstantRange>& ranges)
{
	MurmurHash2 layout_hash;
	layout_hash.addData((const uint8_t*)layouts.data(), (uint32_t)(layouts.size() * sizeof(VkDescriptorSetLayout)));
	layout_hash.addData((const uint8_t*)ranges.data(), (uint32_t)(ranges.size() * sizeof(VkPushConstantRange)));
	uint32_t hash_value = layout_hash.end();

	VkPipelineLayout layout = VK_NULL_HANDLE;
	this->pipeline_map_lock.lock();
	if (has_value(this->pipeline_layouts, hash_value))
	{
		layout = this->pipeline_layouts[hash_value];
	}
	else
	{
		VkPipelineLayoutCreateInfo pipeline_layout_info = {};
		pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_info.setLayoutCount = (uint32_t)layouts.size();
		pipeline_layout_info.pSetLayouts = layouts.data();
		pipeline_layout_info.pushConstantRangeCount = (uint32_t)ranges.size();
		pipeline_layout_info.pPushConstantRanges = ranges.data();
		GENESIS_ENGINE_ASSERT_ERROR((vkCreatePipelineLayout(this->device, &pipeline_layout_info, nullptr, &layout) == VK_SUCCESS), "failed to create pipeline layout");

		this->pipeline_layouts[hash_value] = layout;
	}
	this->pipeline_map_lock.unlock();
	return layout;
}

VkPipelineLayout VulkanLayoutPool::getPipelineLayout(const PipelineLayoutCreateInfo& create_info)
{
	MurmurHash2 layout_hash;
	layout_hash.addData((const uint8_t*)create_info.descriptor_sets, (uint32_t)(create_info.descriptor_sets_count * sizeof(DescriptorSetLayout)));
	layout_hash.addData((const uint8_t*)create_info.push_constant_ranges, (uint32_t)(create_info.push_constant_ranges_count * sizeof(PushConstantRange)));
	uint32_t hash_value = layout_hash.end();

	VkPipelineLayout layout = VK_NULL_HANDLE;
	this->pipeline_map_lock.lock();
	if (has_value(this->pipeline_layouts, hash_value))
	{
		layout = this->pipeline_layouts[hash_value];
	}
	else
	{
		List<VkDescriptorSetLayout> descriptor_sets_layouts(create_info.descriptor_sets_count);
		for (uint32_t i = 0; i < descriptor_sets_layouts.size(); i++)
		{
			descriptor_sets_layouts[i] = (VkDescriptorSetLayout)create_info.descriptor_sets[i];
		}

		List<VkPushConstantRange> push_constant_ranges(create_info.push_constant_ranges_count);
		for (uint32_t i = 0; i < push_constant_ranges.size(); i++)
		{
			push_constant_ranges[i].offset = create_info.push_constant_ranges[i].offset;
			push_constant_ranges[i].size = create_info.push_constant_ranges[i].size;
			push_constant_ranges[i].stageFlags = getShaderStageFlags(create_info.push_constant_ranges[i].stages);
		}

		VkPipelineLayoutCreateInfo pipeline_layout_info = {};
		pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_info.setLayoutCount = (uint32_t)descriptor_sets_layouts.size();
		pipeline_layout_info.pSetLayouts = descriptor_sets_layouts.data();
		pipeline_layout_info.pushConstantRangeCount = (uint32_t)push_constant_ranges.size();
		pipeline_layout_info.pPushConstantRanges = push_constant_ranges.data();
		GENESIS_ENGINE_ASSERT_ERROR((vkCreatePipelineLayout(this->device, &pipeline_layout_info, nullptr, &layout) == VK_SUCCESS), "failed to create pipeline layout");

		this->pipeline_layouts[hash_value] = layout;
	}
	this->pipeline_map_lock.unlock();
	return layout;
}

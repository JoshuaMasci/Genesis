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

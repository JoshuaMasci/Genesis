#include "VulkanDescriptorSetLayouts.hpp"

using namespace Genesis;

VulkanDescriptorSetLayouts::VulkanDescriptorSetLayouts(VkDevice device)
{
	this->device = device;
}

VulkanDescriptorSetLayouts::~VulkanDescriptorSetLayouts()
{
	for (auto layout : this->layouts)
	{
		vkDestroyDescriptorSetLayout(this->device, layout.second, nullptr);
	}
}

VkDescriptorSetLayout VulkanDescriptorSetLayouts::getDescriptorSetLayout(VkDescriptorType type)
{
	//Ignore shader stage for the moment

	//If already exits
	if (this->layouts.find(type) != this->layouts.end())
	{
		return this->layouts[type];
	}

	//else create new one
	VkDescriptorSetLayout layout;

	VkDescriptorSetLayoutBinding layout_binding = {};
	layout_binding.binding = 0;
	layout_binding.descriptorCount = 1;
	layout_binding.descriptorType = type;
	layout_binding.pImmutableSamplers = nullptr;
	layout_binding.stageFlags = VK_SHADER_STAGE_ALL;

	VkDescriptorSetLayoutCreateInfo layout_info = {};
	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_info.bindingCount = 1;
	layout_info.pBindings = &layout_binding;

	if (vkCreateDescriptorSetLayout(this->device, &layout_info, nullptr, &layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}

	//Add to list
	this->layouts[type] = layout;

	return layout;
}

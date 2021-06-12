#pragma once

#include "vulkan_include.hpp"

namespace genesis
{
	class BindlessDescriptor
	{
	protected:
		VkDevice device = VK_NULL_HANDLE;
		VkDescriptorSetLayout descriptor_set_layout = VK_NULL_HANDLE;
		VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
		VkDescriptorSet descriptor_set = VK_NULL_HANDLE;

	public:
		BindlessDescriptor(VkDevice device, vector<VkDescriptorSetLayoutBinding>& bindings);
		~BindlessDescriptor();

		VkDescriptorSetLayout get_descriptor_layout() { return this->descriptor_set_layout; };
		VkDescriptorSet get_descriptor_set() { return this->descriptor_set; };
	};
}
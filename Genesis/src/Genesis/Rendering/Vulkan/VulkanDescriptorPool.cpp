#include "VulkanDescriptorPool.hpp"

#include "Genesis/Core/Array.hpp"

#include <stdexcept>

using namespace Genesis;

VulkanDescriptorPool::VulkanDescriptorPool(VkDevice device, VkDescriptorType type, uint32_t max_number)
{
	this->device = device;

	Array<VkDescriptorPoolSize> pool_sizes(1);
	pool_sizes[0].type = type;
	pool_sizes[0].descriptorCount = max_number;

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
	pool_info.pPoolSizes = pool_sizes.data();
	pool_info.maxSets = max_number;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

	if (vkCreateDescriptorPool(this->device, &pool_info, nullptr, &this->pool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	vkDestroyDescriptorPool(this->device, this->pool, nullptr);
}

VkDescriptorSet VulkanDescriptorPool::getDescriptorSet(VkDescriptorSetLayout layout)
{
	VkDescriptorSet descriptor_set;

	VkDescriptorSetAllocateInfo set_alloc_info = {};
	set_alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	set_alloc_info.descriptorPool = this->pool;
	set_alloc_info.descriptorSetCount = 1;
	set_alloc_info.pSetLayouts = &layout;

	if (vkAllocateDescriptorSets(this->device, &set_alloc_info, &descriptor_set) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	return descriptor_set;
}

void VulkanDescriptorPool::freeDescriptorSet(VkDescriptorSet descriptor_set)
{
	vkFreeDescriptorSets(this->device, this->pool, 1, &descriptor_set);
}

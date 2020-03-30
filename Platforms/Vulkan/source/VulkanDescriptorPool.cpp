#include "VulkanDescriptorPool.hpp"

#include "Genesis/Debug/Assert.hpp"

#include "Genesis/Core/List.hpp"
#include "Genesis/Core/MurmurHash2.hpp"

#include <stdexcept>

using namespace Genesis;

VulkanDescriptorPool::VulkanDescriptorPool(VkDevice device, uint32_t frame_count, uint32_t max_sets, vector<VkDescriptorPoolSize> types)
{
	this->device = device;

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = (uint32_t)types.size();
	pool_info.pPoolSizes = types.data();
	pool_info.maxSets = max_sets;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	GENESIS_ENGINE_ASSERT_ERROR((vkCreateDescriptorPool(this->device, &pool_info, nullptr, &this->pool) == VK_SUCCESS), "failed to create descriptor pool");

	this->frame_data = List<FrameData>(frame_count);
	for (size_t i = 0; i < this->frame_data.size(); i++)
	{
		this->frame_data[i].used_descriptor_set = List<VkDescriptorSet>(MAX_PER_FRAME_DESCRIPTOR_SETS);
	}
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	vkDestroyDescriptorPool(this->device, this->pool, nullptr);
}

VkDescriptorSet VulkanDescriptorPool::getDescriptorSet(VkDescriptorSetLayout layout, uint32_t frame_index)
{
	VkDescriptorSet descriptor_set;

	VkDescriptorSetAllocateInfo set_alloc_info = {};
	set_alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	set_alloc_info.descriptorPool = this->pool;
	set_alloc_info.descriptorSetCount = 1;
	set_alloc_info.pSetLayouts = &layout;
	GENESIS_ENGINE_ASSERT_ERROR((vkAllocateDescriptorSets(this->device, &set_alloc_info, &descriptor_set) == VK_SUCCESS), "failed to allocate descriptor sets");

	//TODO: bounds check and grow array if needed
	FrameData* frame = &this->frame_data[frame_index];
	frame->used_descriptor_set[frame->descriptor_set_count] = descriptor_set;
	frame->descriptor_set_count++;

	return descriptor_set;
}

void VulkanDescriptorPool::resetFrame(uint32_t frame_index)
{
	FrameData* frame = &this->frame_data[frame_index];

	if (frame->descriptor_set_count == 0)
	{
		return;
	}

	vkFreeDescriptorSets(this->device, this->pool, frame->descriptor_set_count, frame->used_descriptor_set.data());
	frame->descriptor_set_count = 0;
}

VulkanDescriptorPool2::VulkanDescriptorPool2(VkDevice device, uint32_t max_sets, vector<VkDescriptorPoolSize> types)
{
	this->device = device;

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = (uint32_t)types.size();
	pool_info.pPoolSizes = types.data();
	pool_info.maxSets = max_sets;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	GENESIS_ENGINE_ASSERT_ERROR((vkCreateDescriptorPool(this->device, &pool_info, nullptr, &this->pool) == VK_SUCCESS), "failed to create descriptor pool");
}

VulkanDescriptorPool2::~VulkanDescriptorPool2()
{
	vkDestroyDescriptorPool(this->device, this->pool, nullptr);
}

VkDescriptorSet VulkanDescriptorPool2::createDescriptorSet(VkDescriptorSetLayout layout)
{
	VkDescriptorSet descriptor_set;

	VkDescriptorSetAllocateInfo set_alloc_info = {};
	set_alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	set_alloc_info.descriptorPool = this->pool;
	set_alloc_info.descriptorSetCount = 1;
	set_alloc_info.pSetLayouts = &layout;
	GENESIS_ENGINE_ASSERT_ERROR((vkAllocateDescriptorSets(this->device, &set_alloc_info, &descriptor_set) == VK_SUCCESS), "failed to allocate descriptor sets");

	return descriptor_set;
}

void VulkanDescriptorPool2::destroyDescriptorSet(VkDescriptorSet descriptor_set)
{
	GENESIS_ENGINE_ASSERT_ERROR((vkFreeDescriptorSets(this->device, this->pool, 1, &descriptor_set) == VK_SUCCESS), "failed to free descriptor sets");
}

Genesis::VulkanPreFrameDescriptorPool::VulkanPreFrameDescriptorPool(VkDevice device, uint32_t max_sets, vector<VkDescriptorPoolSize> types)
{
	this->device = device;

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = (uint32_t)types.size();
	pool_info.pPoolSizes = types.data();
	pool_info.maxSets = max_sets;
	pool_info.flags = 0;
	GENESIS_ENGINE_ASSERT_ERROR((vkCreateDescriptorPool(this->device, &pool_info, nullptr, &this->pool) == VK_SUCCESS), "failed to create descriptor pool");
}

Genesis::VulkanPreFrameDescriptorPool::~VulkanPreFrameDescriptorPool()
{
	vkDestroyDescriptorPool(this->device, this->pool, nullptr);
}

VkDescriptorSet Genesis::VulkanPreFrameDescriptorPool::createDescriptorSet(VkDescriptorSetLayout layout)
{
	VkDescriptorSet descriptor_set;

	VkDescriptorSetAllocateInfo set_alloc_info = {};
	set_alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	set_alloc_info.descriptorPool = this->pool;
	set_alloc_info.descriptorSetCount = 1;
	set_alloc_info.pSetLayouts = &layout;
	GENESIS_ENGINE_ASSERT_ERROR((vkAllocateDescriptorSets(this->device, &set_alloc_info, &descriptor_set) == VK_SUCCESS), "failed to allocate descriptor sets");

	return descriptor_set;
}

void Genesis::VulkanPreFrameDescriptorPool::resetPool()
{
	vkResetDescriptorPool(this->device, this->pool, 0);
}

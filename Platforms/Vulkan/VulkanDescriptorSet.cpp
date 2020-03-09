#include "VulkanDescriptorSet.hpp"

using namespace Genesis;

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDescriptorPool2* descriptor_pool, const DescriptorSetCreateInfo& create_info)
{
	this->descriptor_pool = descriptor_pool;

	this->descriptor_set = this->descriptor_pool->createDescriptorSet((VkDescriptorSetLayout)create_info.layout);
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
	this->descriptor_pool->destroyDescriptorSet(this->descriptor_set);
}

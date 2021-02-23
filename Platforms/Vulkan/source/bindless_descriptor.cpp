#include "bindless_descriptor.hpp"

namespace genesis
{
	BindlessDescriptor::BindlessDescriptor(VkDevice device, vector<VkDescriptorSetLayoutBinding>& bindings)
	{
		this->device = device;

		VkDescriptorSetLayoutCreateInfo layout_create_info = {};
		layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_create_info.pNext = NULL;
		layout_create_info.flags = 0;
		layout_create_info.bindingCount = (uint32_t)bindings.size();
		layout_create_info.pBindings = bindings.data();
		VK_ASSERT(vkCreateDescriptorSetLayout(this->device, &layout_create_info, nullptr, &this->descriptor_set_layout));


		vector<VkDescriptorPoolSize> pool_sizes;
		pool_sizes.reserve(bindings.size());
		for (auto binding : bindings)
		{
			VkDescriptorPoolSize size = {};
			size.type = binding.descriptorType;
			size.descriptorCount = binding.descriptorCount;
			pool_sizes.push_back(size);
		}

		VkDescriptorPoolCreateInfo pool_create_info = {};
		pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_create_info.poolSizeCount = (uint32_t)pool_sizes.size();
		pool_create_info.pPoolSizes = pool_sizes.data();
		pool_create_info.maxSets = 1;
		VK_ASSERT(vkCreateDescriptorPool(this->device, &pool_create_info, nullptr, &this->descriptor_pool));

		VkDescriptorSetAllocateInfo set_alloc_info = {};
		set_alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		set_alloc_info.descriptorPool = this->descriptor_pool;
		set_alloc_info.descriptorSetCount = 1;
		set_alloc_info.pSetLayouts = &this->descriptor_set_layout;
		VK_ASSERT(vkAllocateDescriptorSets(this->device, &set_alloc_info, &this->descriptor_set));
	}

	BindlessDescriptor::~BindlessDescriptor()
	{
		vkDestroyDescriptorPool(this->device, this->descriptor_pool, nullptr);
		vkDestroyDescriptorSetLayout(this->device, this->descriptor_set_layout, nullptr);
	}
}
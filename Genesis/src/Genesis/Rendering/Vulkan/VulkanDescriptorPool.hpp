#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"

//#include <concurrentqueue.h>

namespace Genesis
{
	class VulkanDescriptorPool
	{
	public:
		VulkanDescriptorPool(VkDevice device, VkDescriptorType type, uint32_t max_number);
		~VulkanDescriptorPool();

		inline VkDescriptorPool get() { return this->pool; };

		VkDescriptorSet getDescriptorSet(VkDescriptorSetLayout layout);
		void freeDescriptorSet(VkDescriptorSet descriptor_set);

	private:
		VkDevice device;
		VkDescriptorPool pool;
	};
}
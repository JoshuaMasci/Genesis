#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"

//#include <concurrentqueue.h>

namespace Genesis
{
	class VulkanDescriptorPool
	{
	public:
		VulkanDescriptorPool(VkDevice device, VkDescriptorType type, VkDescriptorSetLayout layout, uint32_t max_number);
		~VulkanDescriptorPool();

		inline VkDescriptorPool get() { return this->pool; };

		VkDescriptorSet getDescriptorSet();
		void freeDescriptorSet(VkDescriptorSet descriptor_set);

	private:
		VkDevice device;
		VkDescriptorPool pool;
		VkDescriptorSetLayout layout;

		//moodycamel::ConcurrentQueue<VkDescriptorSet> descriptor_set_queue;
	};
}
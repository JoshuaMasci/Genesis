#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	class VulkanDescriptorPool
	{
	public:
		VulkanDescriptorPool(VkDevice device, VkDescriptorType type, uint32_t max_number);
		VulkanDescriptorPool(VkDevice device, uint32_t max_sets, vector<VkDescriptorPoolSize> types);
		~VulkanDescriptorPool();

		inline VkDescriptorPool get() { return this->pool; };

		VkDescriptorSet getDescriptorSet(VkDescriptorSetLayout layout);
		void freeDescriptorSet(VkDescriptorSet descriptor_set);

	private:
		VkDevice device;
		VkDescriptorPool pool;
	};
}
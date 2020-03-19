#pragma once

#include "VulkanInclude.hpp"
#include "Genesis/Core/Types.hpp"
#include "VulkanDescriptorPool.hpp"
#include "Genesis/RenderingBackend/DescriptorSet.hpp"

namespace Genesis
{
	class VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet(VulkanDescriptorPool2* descriptor_pool, const DescriptorSetCreateInfo& create_info);
		~VulkanDescriptorSet();

		inline VkDescriptorSet get() { return this->descriptor_set; };

	private:
		VkDescriptorSet descriptor_set;
		VulkanDescriptorPool2* descriptor_pool;
	};
}
#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	class VulkanDescriptorSetLayouts
	{
	public:
		VulkanDescriptorSetLayouts(VkDevice device);
		~VulkanDescriptorSetLayouts();

		VkDescriptorSetLayout getDescriptorSetLayout(VkDescriptorType type);

	private:
		VkDevice device;

		map<VkDescriptorType, VkDescriptorSetLayout> layouts;
	};
}
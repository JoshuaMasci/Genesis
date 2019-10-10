#pragma once

#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDescriptorPool.hpp"

namespace Genesis
{
	struct VulkanThread
	{
		VulkanCommandPool* secondary_graphics_pool;
		VulkanDescriptorPool* descriptor_pool;
	};
}
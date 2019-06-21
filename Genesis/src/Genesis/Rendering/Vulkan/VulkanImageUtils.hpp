#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"

namespace Genesis
{
	void transitionImageLayout(VulkanCommandPool* command_pool, VkQueue queue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VulkanCommandPool* command_pool, VkQueue queue, VkBuffer buffer, VkImage image, VkExtent2D size);
}
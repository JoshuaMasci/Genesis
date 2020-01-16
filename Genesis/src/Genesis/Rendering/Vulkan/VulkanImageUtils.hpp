#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"

namespace Genesis
{
	void transitionImageLayout(VkCommandBuffer transfer_buffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkCommandBuffer transfer_buffer, VkBuffer buffer, VkImage image, VkExtent2D size);
}
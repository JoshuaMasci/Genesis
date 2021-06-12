#pragma once

#include "vulkan_include.hpp"
#include "device.hpp"

namespace genesis
{
	class Buffer
	{
	protected:
		Device* device = nullptr;
		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation memory = VK_NULL_HANDLE;
		VkDeviceSize size = 0;

	public:
		Buffer(Device* device, const VkBufferCreateInfo& create_info, VmaMemoryUsage memory_usage);
		~Buffer();
	};
}
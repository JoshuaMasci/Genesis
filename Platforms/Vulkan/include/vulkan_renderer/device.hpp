#pragma once

#include "vulkan_include.hpp"
#include "genesis_core/types/containers.hpp"

namespace genesis
{
	class Device
	{
	protected:
		VkPhysicalDevice physical_device = VK_NULL_HANDLE;
		VkDevice logical_device = VK_NULL_HANDLE;
		VkQueue primary_queue = VK_NULL_HANDLE;
		uint32_t primary_queue_index;

		VmaAllocator allocator;

	public:
		Device(VkInstance instance, VkPhysicalDevice physical_device, vector<const char*>& extensions, vector<const char*>& layers);
		~Device();

		inline VkDevice get() { return this->logical_device; };
		inline VkPhysicalDevice get_physical_device() { return this->physical_device; };
		inline VkQueue get_queue() { return this->primary_queue; };
		inline uint32_t get_queue_index() { return this->primary_queue_index; };

		bool is_memory_host_visible(VmaAllocationInfo& memory_info);
		void map_memory(VmaAllocation memory, void** data);
		void unmap_memory(VmaAllocation memory);

		void create_buffer(const VkBufferCreateInfo* buffer_create, VmaMemoryUsage memory_usage, VkBuffer* buffer, VmaAllocation* memory, VmaAllocationInfo* memory_info);
		void destroy_buffer(VkBuffer buffer, VmaAllocation memory);

		void create_image(const VkImageCreateInfo* image_create, VmaMemoryUsage memory_usage, VkImage* image, VmaAllocation* memory, VmaAllocationInfo* memory_info);
		void destroy_image(VkImage image, VmaAllocation memory);

		VkSemaphore create_semaphore();
		void destroy_semaphore(VkSemaphore semaphore);

		VkFence create_fence(bool signaled = true);
		void destroy_fence(VkFence fence);

		VkEvent create_event();
		void destroy_event(VkEvent vk_event);

		inline void wait_idle() { vkDeviceWaitIdle(this->logical_device); };
	};
}
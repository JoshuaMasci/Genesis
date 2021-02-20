#pragma once

#include "Genesis/Core/Types.hpp"
#include "VulkanInclude.hpp"

#include "vk_mem_alloc.h"

namespace Genesis
{
	class VulkanDevice
	{
	public:
		//Device
		VulkanDevice(VkPhysicalDevice device, VkSurfaceKHR surface, vector<const char*>& extensions, vector<const char*>& layers);
		~VulkanDevice();

		inline VkDevice get() { return this->logical_device; };
		inline VkPhysicalDevice getPhysicalDevice() { return this->physical_device; };

		//Device Memory
		bool isMemoryHostVisible(VmaAllocationInfo& memory_info);
		void mapMemory(VmaAllocation memory, void** data);
		void unmapMemory(VmaAllocation memory);

		void createBuffer(VkBufferCreateInfo* buffer_create, VmaMemoryUsage memory_usage, VkBuffer* buffer, VmaAllocation* memory, VmaAllocationInfo* memory_info);
		void destroyBuffer(VkBuffer buffer, VmaAllocation memory);

		void createImage(VkImageCreateInfo* image_create, VmaMemoryUsage memory_usage, VkImage* image, VmaAllocation* memory, VmaAllocationInfo* memory_info);
		void destroyImage(VkImage image, VmaAllocation memory);

		VkSemaphore createSemaphore();
		void destroySemaphore(VkSemaphore semaphore);

		VkFence createFence();
		void destroyFence(VkFence fence);

		VkEvent createEvent();
		void destroyEvent(VkEvent vk_event);

		//Device Queues
		inline VkQueue getGraphicsQueue() { return this->graphics_queue; };
		inline uint32_t getGraphicsFamilyIndex() { return this->graphics_family_index; };

		inline VkQueue getPresentQueue() { return this->present_queue; };
		inline uint32_t getPresentFamilyIndex() { return this->present_family_index; };

		inline VkQueue getTransferQueue() { return this->transfer_queue; };
		inline uint32_t getTransferFamilyIndex() { return this->transfer_family_index; };

		inline VkQueue getComputeQueue() { return this->compute_queue; };
		inline uint32_t getComputeFamilyIndex() { return this->compute_family_index; };

		inline void waitIdle() { vkDeviceWaitIdle(this->logical_device); };

	private:
		//Device
		VkPhysicalDevice physical_device;
		VkDevice logical_device;
		
		//Device Properties
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceMemoryProperties memory_properties;
		VkPhysicalDeviceFeatures features;

		//Device Memory
		VmaAllocator allocator;

		//Device Queues
		VkQueue graphics_queue;
		uint32_t graphics_family_index;

		VkQueue present_queue;
		uint32_t present_family_index;

		VkQueue compute_queue;
		uint32_t compute_family_index;

		VkQueue transfer_queue;
		uint32_t transfer_family_index;
	};	
}
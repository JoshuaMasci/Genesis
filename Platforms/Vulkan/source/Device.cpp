#define VMA_IMPLEMENTATION

#include "Device.hpp"

namespace VulkanRenderer
{
	Device::Device(VkInstance instance, VkPhysicalDevice physical_device, std::vector<const char*>& extensions, std::vector<const char*>& layers)
	{
		VkPhysicalDeviceFeatures device_features = {};
		//No device features needed as of now

		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.pQueueCreateInfos = nullptr;
		create_info.queueCreateInfoCount = 0;

		create_info.pEnabledFeatures = &device_features;

		create_info.enabledExtensionCount = 0;

		if (extensions.size() > 0)
		{
			create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			create_info.ppEnabledExtensionNames = extensions.data();
		}
		else
		{
			create_info.enabledExtensionCount = 0;
		}

		if (layers.size() > 0)
		{
			create_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
			create_info.ppEnabledLayerNames = layers.data();
		}
		else
		{
			create_info.enabledLayerCount = 0;
		}

		this->primary_queue_index = 0;

		float queue_priorities = 1.0f;
		VkDeviceQueueCreateInfo primary_queue = {};
		primary_queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		primary_queue.queueFamilyIndex = this->primary_queue_index;
		primary_queue.queueCount = 1;
		primary_queue.pQueuePriorities = &queue_priorities;

		create_info.pQueueCreateInfos = &primary_queue;
		create_info.queueCreateInfoCount = 1;

		this->physical_device = physical_device;
		VK_ASSERT(vkCreateDevice(this->physical_device, &create_info, nullptr, &this->logical_device));

		vkGetDeviceQueue(this->logical_device, this->primary_queue_index, 0, &this->primary_queue);

		uint32_t vk_version = 0;
		vkEnumerateInstanceVersion(&vk_version);
		VmaAllocatorCreateInfo allocator_info = {};
		allocator_info.vulkanApiVersion = VK_API_VERSION_1_0;
		allocator_info.physicalDevice = this->physical_device;
		allocator_info.device = this->logical_device;
		allocator_info.instance = instance;
		vmaCreateAllocator(&allocator_info, &this->allocator);
	}

	Device::~Device()
	{
		vmaDestroyAllocator(this->allocator);
		vkDestroyDevice(this->logical_device, nullptr);
	}

	bool Device::is_memory_host_visible(VmaAllocationInfo& memory_info)
	{
		return false;
	}

	void Device::map_memory(VmaAllocation memory, void** data)
	{
		vmaMapMemory(this->allocator, memory, data);
	}

	void Device::unmap_memory(VmaAllocation memory)
	{
		vmaUnmapMemory(this->allocator, memory);
	}

	void Device::create_buffer(const VkBufferCreateInfo* buffer_create, VmaMemoryUsage memory_usage, VkBuffer* buffer, VmaAllocation* memory, VmaAllocationInfo* memory_info)
	{
		VmaAllocationCreateInfo alloc_info = {};
		alloc_info.usage = memory_usage;
		VK_ASSERT(vmaCreateBuffer(this->allocator, buffer_create, &alloc_info, buffer, memory, memory_info));
	}

	void Device::destroy_buffer(VkBuffer buffer, VmaAllocation memory)
	{
		vmaDestroyBuffer(this->allocator, buffer, memory);
	}

	void Device::create_image(const VkImageCreateInfo* image_create, VmaMemoryUsage memory_usage, VkImage* image, VmaAllocation* memory, VmaAllocationInfo* memory_info)
	{
		VmaAllocationCreateInfo alloc_info = {};
		alloc_info.usage = memory_usage;
		VK_ASSERT(vmaCreateImage(this->allocator, image_create, &alloc_info, image, memory, memory_info));
	}

	void Device::destroy_image(VkImage image, VmaAllocation memory)
	{
		vmaDestroyImage(this->allocator, image, memory);
	}

	VkSemaphore Device::create_semaphore()
	{
		VkSemaphore semaphore;
		VkSemaphoreCreateInfo semaphore_info = {};
		semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VK_ASSERT(vkCreateSemaphore(this->logical_device, &semaphore_info, nullptr, &semaphore));
		return semaphore;
	}

	void Device::destroy_semaphore(VkSemaphore semaphore)
	{
		vkDestroySemaphore(this->logical_device, semaphore, nullptr);
	}

	VkFence Device::create_fence(bool signaled)
	{
		VkFence fence;
		VkFenceCreateInfo fence_info = {};
		fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_info.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
		VK_ASSERT(vkCreateFence(this->logical_device, &fence_info, nullptr, &fence));
		return fence;
	}

	void Device::destroy_fence(VkFence fence)
	{
		vkDestroyFence(this->logical_device, fence, nullptr);
	}

	VkEvent Device::create_event()
	{
		VkEvent event;
		VkEventCreateInfo event_info = {};
		event_info.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
		VK_ASSERT(vkCreateEvent(this->logical_device, &event_info, nullptr, &event));
		return event;
	}

	void Device::destroy_event(VkEvent vk_event)
	{
		vkDestroyEvent(this->logical_device, vk_event, nullptr);
	}
}
#define VMA_IMPLEMENTATION

#include "Genesis/Debug/Assert.hpp"

#include "VulkanDevice.hpp"

#include "Genesis/Debug/Log.hpp"
#include "VulkanQueueFamily.hpp"

using namespace Genesis;

VulkanDevice::VulkanDevice(VkPhysicalDevice device, VkSurfaceKHR surface, vector<const char*>& extensions, vector<const char*>& layers)
{
	this->physical_device = device;
	vkGetPhysicalDeviceProperties(this->physical_device, &this->properties);
	vkGetPhysicalDeviceMemoryProperties(this->physical_device, &this->memory_properties);
	vkGetPhysicalDeviceFeatures(this->physical_device, &this->features);

	GENESIS_ENGINE_INFO("GPU INFO");
	GENESIS_ENGINE_INFO("Device: {}", this->properties.deviceName);

	VulkanQueueFamilyAllocator queue_allocator(this->physical_device, surface);
	vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	//Fill queue priorities with the required number of 1.0fs
	vector<float> queue_priority(1);
	for (QueueFamilyUseage queueFamily : queue_allocator.queue_families)
	{
		if (queue_priority.size() < queueFamily.queues_needed)
		{
			queue_priority.resize(queueFamily.queues_needed);
		}
	}

	for (uint32_t i = 0; i < queue_priority.size(); i++)
	{
		queue_priority[i] = 1.0f;
	}

	for (QueueFamilyUseage queueFamily : queue_allocator.queue_families)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily.queue_family;
		queueCreateInfo.queueCount = queueFamily.queues_needed;
		queueCreateInfo.pQueuePriorities = queue_priority.data();
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures device_features = {};
	//No device features needed as of now
	//TODO add as needed

	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	create_info.pQueueCreateInfos = queueCreateInfos.data();
	create_info.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

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

	GENESIS_ENGINE_ASSERT_ERROR((vkCreateDevice(this->physical_device, &create_info, nullptr, &this->logical_device) == VK_SUCCESS), "failed to create logical device");

	vkGetDeviceQueue(this->logical_device, queue_allocator.graphics.queue_family, queue_allocator.graphics.queue_index, &this->graphics_queue);
	vkGetDeviceQueue(this->logical_device, queue_allocator.present.queue_family, queue_allocator.present.queue_index, &this->present_queue);
	vkGetDeviceQueue(this->logical_device, queue_allocator.transfer.queue_family, queue_allocator.transfer.queue_index, &this->transfer_queue);
	vkGetDeviceQueue(this->logical_device, queue_allocator.compute.queue_family, queue_allocator.compute.queue_index, &this->compute_queue);

	this->graphics_family_index = queue_allocator.graphics.queue_family;
	this->present_family_index = queue_allocator.present.queue_family;
	this->transfer_family_index = queue_allocator.transfer.queue_family;
	this->compute_family_index = queue_allocator.compute.queue_family;

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = this->physical_device;
	allocatorInfo.device = this->logical_device;
	vmaCreateAllocator(&allocatorInfo, &this->allocator);
}

VulkanDevice::~VulkanDevice()
{
	vmaDestroyAllocator(this->allocator);
	vkDestroyDevice(this->logical_device, nullptr);
}

bool Genesis::VulkanDevice::isMemoryHostVisible(VmaAllocationInfo& memory_info)
{
	VkMemoryPropertyFlags memFlags;
	vmaGetMemoryTypeProperties(this->allocator, memory_info.memoryType, &memFlags);
	return (memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0;
}

void VulkanDevice::mapMemory(VmaAllocation memory, void** data)
{
	vmaMapMemory(this->allocator, memory, data);
}

void VulkanDevice::unmapMemory(VmaAllocation memory)
{
	vmaUnmapMemory(this->allocator, memory);
}

void VulkanDevice::createBuffer(VkBufferCreateInfo* buffer_create, VmaMemoryUsage memory_usage, VkBuffer * buffer, VmaAllocation * memory, VmaAllocationInfo * memory_info)
{
	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage = memory_usage;

	GENESIS_ENGINE_ASSERT_ERROR((vmaCreateBuffer(this->allocator, buffer_create, &alloc_info, buffer, memory, memory_info) == VK_SUCCESS), "failed to create buffer");
}

void VulkanDevice::destroyBuffer(VkBuffer buffer, VmaAllocation memory)
{
	vmaDestroyBuffer(this->allocator, buffer, memory);
}

void VulkanDevice::createImage(VkImageCreateInfo* image_create, VmaMemoryUsage memory_usage, VkImage * image, VmaAllocation * memory, VmaAllocationInfo * memory_info)
{
	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage = memory_usage;
	GENESIS_ENGINE_ASSERT_ERROR((vmaCreateImage(this->allocator, image_create, &alloc_info, image, memory, memory_info) == VK_SUCCESS), "failed to create image");
}

void VulkanDevice::destroyImage(VkImage image, VmaAllocation memory)
{
	vmaDestroyImage(this->allocator, image, memory);
}

VkSemaphore VulkanDevice::createSemaphore()
{
	VkSemaphore semaphore;
	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	GENESIS_ENGINE_ASSERT_ERROR((vkCreateSemaphore(this->logical_device, &semaphore_info, nullptr, &semaphore) == VK_SUCCESS), "failed to create semaphore");
	return semaphore;
}

void VulkanDevice::destroySemaphore(VkSemaphore semaphore)
{
	vkDestroySemaphore(this->logical_device, semaphore, nullptr);
}

VkFence VulkanDevice::createFence()
{
	VkFence fence;
	VkFenceCreateInfo fence_info = {};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	GENESIS_ENGINE_ASSERT_ERROR((vkCreateFence(this->logical_device, &fence_info, nullptr, &fence) == VK_SUCCESS), "failed to create fence");
	return fence;
}

void VulkanDevice::destroyFence(VkFence fence)
{
	vkDestroyFence(this->logical_device, fence, nullptr);
}

VkEvent VulkanDevice::createEvent()
{
	VkEventCreateInfo event_info = {};
	event_info.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;

	VkEvent vk_event;
	vkCreateEvent(this->logical_device, &event_info, nullptr, &vk_event);

	return vk_event;
}

void VulkanDevice::destroyEvent(VkEvent vk_event)
{
	vkDestroyEvent(this->logical_device, vk_event, nullptr);
}

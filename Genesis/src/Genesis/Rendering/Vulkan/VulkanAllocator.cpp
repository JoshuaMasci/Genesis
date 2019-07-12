#define VMA_IMPLEMENTATION

#include "VulkanAllocator.hpp"

using namespace Genesis;

VulkanAllocator::VulkanAllocator(VulkanDevice* device)
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = device->getPhysicalDevice();
	allocatorInfo.device = device->get();

	vmaCreateAllocator(&allocatorInfo, &this->allocator);
}

VulkanAllocator::~VulkanAllocator()
{
	vmaDestroyAllocator(this->allocator);
}

bool VulkanAllocator::isMemoryHostVisible(VmaAllocationInfo & memory_info)
{
	VkMemoryPropertyFlags memFlags;
	vmaGetMemoryTypeProperties(this->allocator, memory_info.memoryType, &memFlags);
	return (memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0;
}

void VulkanAllocator::mapMemory(VmaAllocation memory, void** data)
{
	vmaMapMemory(this->allocator, memory, data);
}

void VulkanAllocator::unmapMemory(VmaAllocation memory)
{
	vmaUnmapMemory(this->allocator, memory);
}

void VulkanAllocator::createBuffer(VkBufferCreateInfo* buffer_create, VmaMemoryUsage memory_usage, VkBuffer* buffer, VmaAllocation* memory, VmaAllocationInfo* memory_info)
{
	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage = memory_usage;

	vmaCreateBuffer(this->allocator, buffer_create, &alloc_info, buffer, memory, memory_info);
}

void VulkanAllocator::destroyBuffer(VkBuffer buffer, VmaAllocation memory)
{
	vmaDestroyBuffer(this->allocator, buffer, memory);
}

void VulkanAllocator::createImage(VkImageCreateInfo* image_create, VmaMemoryUsage memory_usage, VkImage* image, VmaAllocation* memory, VmaAllocationInfo* memory_info)
{
	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage = memory_usage;

	VkResult result = vmaCreateImage(this->allocator, image_create, &alloc_info, image, memory, memory_info);
	if (result != VK_SUCCESS)
	{
		printf("Error\n");
	}
}

void VulkanAllocator::destroyImage(VkImage image, VmaAllocation memory)
{
	vmaDestroyImage(this->allocator, image, memory);
}

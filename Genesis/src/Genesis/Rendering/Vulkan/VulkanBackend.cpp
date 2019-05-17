#include "VulkanBackend.hpp"

#define VMA_IMPLEMENTATION

#include "Genesis/Rendering/Vulkan/VulkanInstance.hpp"

#include "Genesis/Rendering/Vulkan/VulkanBuffer.hpp"

using namespace Genesis;

VkBufferUsageFlags getBufferUsage(BufferType usage)
{
	switch (usage)
	{
	case BufferType::Uniform:
		return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	case BufferType::Index:
		return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	case BufferType::Vertex:
		return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	}
	return 0;
}

VmaMemoryUsage getMemoryUsage(MemoryUsage memory_usage)
{
	switch (memory_usage)
	{
	case MemoryUsage::GPU_Only:
		return VMA_MEMORY_USAGE_GPU_ONLY;
	case MemoryUsage::CPU_Visable:
		return VMA_MEMORY_USAGE_CPU_TO_GPU;//Subject To Change
	}
	return VMA_MEMORY_USAGE_UNKNOWN;
}

VulkanBackend::VulkanBackend(Window* window)
{
	this->vulkan = new VulkanInstance(window);
}

VulkanBackend::~VulkanBackend()
{
	delete this->vulkan;
}

uint32_t VulkanBackend::createImage(vector2U size)
{
	return uint32_t();
}

Buffer* VulkanBackend::createBuffer(uint32_t size_bytes, BufferType type, MemoryUsage memory_usage)
{
	return new VulkanBuffer(this->vulkan->allocator, size_bytes, getBufferUsage(type), getMemoryUsage(memory_usage));
}

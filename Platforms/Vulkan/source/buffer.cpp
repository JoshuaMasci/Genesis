#include "vulkan_renderer/buffer.hpp"

namespace genesis
{
	Buffer::Buffer(Device* device, const VkBufferCreateInfo& create_info, VmaMemoryUsage memory_usage)
	{
		this->device = device;
		this->size = create_info.size;

		VmaAllocationInfo allocation_info;
		this->device->create_buffer(&create_info, memory_usage, &this->buffer, &this->memory, &allocation_info);
	}

	Buffer::~Buffer()
	{
		if (this->device != nullptr)
		{
			this->device->destroy_buffer(this->buffer, this->memory);
		}
	}
}
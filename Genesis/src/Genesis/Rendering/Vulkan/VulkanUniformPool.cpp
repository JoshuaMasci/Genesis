#include "VulkanUniformPool.hpp"

using namespace Genesis;

VulkanUniformPool::VulkanUniformPool(VulkanAllocator* allocator, uint32_t frame_count)
{
	this->allocator = allocator;
	this->frame_queues = vector<moodycamel::ConcurrentQueue<VulkanBuffer*>>(frame_count);
}

VulkanUniformPool::~VulkanUniformPool()
{
	for (uint32_t i = 0; i < this->frame_queues.size(); i++)
	{
		this->resetFrame(i);
	}
	
	size_t size = this->main_queue.size_approx();
	Array<VulkanBuffer*> all_buffers(size);
	size_t real_size = this->main_queue.try_dequeue_bulk(all_buffers.data(), size);

	for (size_t i = 0; i < real_size; i++)
	{
		delete all_buffers[i];
	}
}

void VulkanUniformPool::resetFrame(uint32_t frame_index)
{
	size_t size = this->frame_queues[frame_index].size_approx();
	Array<VulkanBuffer*> used_buffers(size);
	size_t real_size = this->frame_queues[frame_index].try_dequeue_bulk(used_buffers.data(), size);
	this->main_queue.enqueue_bulk(used_buffers.data(), real_size);
}

VulkanBuffer* VulkanUniformPool::getBuffer(uint32_t frame_index, uint64_t buffer_size)
{
	VulkanBuffer* buffer = nullptr;
	if (!this->main_queue.try_dequeue(buffer))
	{
		buffer = new VulkanBuffer(this->allocator, 64, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	}

	this->frame_queues[frame_index].enqueue(buffer);

	return buffer;
}

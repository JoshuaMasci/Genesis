#include "VulkanUniformPool.hpp"

using namespace Genesis;

VulkanUniformPool::VulkanUniformPool(VulkanAllocator* allocator, uint32_t frame_count)
{
	this->allocator = allocator;

	const vector<uint16_t> buffer_sizes = BUFFER_SIZES;
	this->pools.resize(buffer_sizes.size());

	for (size_t i = 0; i < this->pools.size(); i++)
	{
		this->pools[i].size = buffer_sizes[i];
		this->pools[i].frame_queues = vector<moodycamel::ConcurrentQueue<VulkanBuffer*>>(frame_count);
	}
}

VulkanUniformPool::~VulkanUniformPool()
{
	for (uint32_t i = 0; i < this->pools[0].frame_queues.size(); i++)
	{
		this->resetFrame(i);
	}
	
	for (size_t i = 0; i < this->pools.size(); i++)
	{
		VulkanUniformPoolSet* pool_set = &this->pools[i];

		size_t size = pool_set->main_queue.size_approx();
		Array<VulkanBuffer*> all_buffers(size);
		size_t real_size = pool_set->main_queue.try_dequeue_bulk(all_buffers.data(), size);

		for (size_t i = 0; i < real_size; i++)
		{
			delete all_buffers[i];
		}
	}
}

void VulkanUniformPool::resetFrame(uint32_t frame_index)
{
	for (size_t i = 0; i < this->pools.size(); i++)
	{
		VulkanUniformPoolSet* pool_set = &this->pools[i];

		size_t size = pool_set->frame_queues[frame_index].size_approx();
		Array<VulkanBuffer*> used_buffers(size);
		size_t real_size = pool_set->frame_queues[frame_index].try_dequeue_bulk(used_buffers.data(), size);
		pool_set->main_queue.enqueue_bulk(used_buffers.data(), real_size);
	}
}

size_t getBufferIndex(uint64_t size)
{
	const vector<uint16_t> buffer_sizes = BUFFER_SIZES;

	for (size_t i = 0; i < buffer_sizes.size(); i++)
	{
		if (size <= buffer_sizes[i])
		{
			return i;
		}
	}

	throw std::runtime_error("Uniform Buffer size too big");
}

VulkanBuffer* VulkanUniformPool::getBuffer(uint32_t frame_index, uint64_t buffer_size)
{
	size_t index = getBufferIndex(buffer_size);
	VulkanUniformPoolSet* pool_set = &this->pools[index];

	VulkanBuffer* buffer = nullptr;
	if (!pool_set->main_queue.try_dequeue(buffer))
	{
		buffer = new VulkanBuffer(this->allocator, pool_set->size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	}

	pool_set->frame_queues[frame_index].enqueue(buffer);

	return buffer;
}

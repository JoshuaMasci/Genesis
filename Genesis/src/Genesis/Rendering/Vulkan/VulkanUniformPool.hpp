#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanBuffer.hpp"
#include "Genesis/Core/Types.hpp"

#include <concurrentqueue.h> 

#define BUFFER_SIZES {16,32,64,128,256,512,1024,2048,4096}

namespace Genesis
{
	class VulkanUniformPool
	{
	public:
		VulkanUniformPool(VulkanAllocator* allocator, uint32_t frame_count);
		~VulkanUniformPool();

		void resetFrame(uint32_t frame_index);

		VulkanBuffer* getBuffer(uint32_t frame_index, uint64_t buffer_size);

	private:
		VulkanAllocator* allocator;

		struct VulkanUniformPoolSet
		{
			uint16_t size;
			moodycamel::ConcurrentQueue<VulkanBuffer*> main_queue;
			vector<moodycamel::ConcurrentQueue<VulkanBuffer*>> frame_queues;
		};

		vector<VulkanUniformPoolSet> pools;
	};
}
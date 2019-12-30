#pragma once

#include <mutex>

#include "Genesis/Rendering/Vulkan/VulkanBuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanTexture.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanCommandPool.hpp"

namespace Genesis
{
	class VulkanTransferBuffer
	{
	public:
		VulkanTransferBuffer(VulkanDevice* device, VulkanCommandPool* transfer_pool);
		~VulkanTransferBuffer();

		void reset();
		bool SubmitTransfers(VkQueue queue);

		void copyBuffer(VulkanBuffer* source, uint64_t source_offset, VulkanBuffer* destination, uint64_t destination_offset, uint64_t copy_size, bool delete_source = true);
		void fillTexture(VulkanBuffer* source, VulkanTexture* destination);

		inline VkSemaphore getTransferDoneSemaphore() { return this->transfer_done_semaphore; };

	private:
		struct CopyBuffer
		{
			VkBuffer source;
			VkBuffer destination;
			VkBufferCopy region;
		};

		struct FillTexture
		{
			VkBuffer source;
			VkImage destination;

			VkExtent2D size;
			VkFormat format; 
			VkImageLayout old_layout; 
			VkImageLayout new_layout;
		};

		VulkanDevice* device = nullptr;
		VulkanCommandPool* transfer_pool = nullptr;

		std::mutex buffer_lock;

		VkCommandBuffer transfer_buffer;
		uint32_t transfer_count = 0;

		uint32_t delete_list_index = 0;
		List<VulkanBuffer*> delete_list;

		uint32_t copy_buffer_list_index = 0;
		List<CopyBuffer> copy_buffer_list;

		uint32_t fill_texture_list_index = 0;
		List<FillTexture> fill_texture_list;

		VkSemaphore transfer_done_semaphore = VK_NULL_HANDLE;
	};
}
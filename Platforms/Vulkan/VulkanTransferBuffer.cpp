#include "VulkanTransferBuffer.hpp"

#include "Genesis/Debug/Assert.hpp"

#include "VulkanImageUtils.hpp"

using namespace Genesis;

VulkanTransferBuffer::VulkanTransferBuffer(VulkanDevice* device, VulkanCommandPool* transfer_pool)
{
	this->device = device;
	this->transfer_pool = transfer_pool;

	this->copy_buffer_list.resize(1000);//TEMP SIZE
	this->fill_texture_list.resize(1000);

	this->transfer_buffer = this->transfer_pool->getCommandBuffer();

	this->transfer_done_semaphore = this->device->createSemaphore();
}

VulkanTransferBuffer::~VulkanTransferBuffer()
{
	this->transfer_pool->freeCommandBuffer(this->transfer_buffer);
	this->device->destroySemaphore(this->transfer_done_semaphore);
}

void VulkanTransferBuffer::reset()
{
	this->buffer_lock.lock();

	if (this->transfer_count == 0)
	{
		this->copy_buffer_list_index = 0;
		this->fill_texture_list_index = 0;
	}

	this->buffer_lock.unlock();
}

/*
	Returns true if was submitted else false;
*/
bool VulkanTransferBuffer::SubmitTransfers(VkQueue queue)
{
	this->buffer_lock.lock();

	vkResetCommandBuffer(this->transfer_buffer, 0);

	bool submitted = false;
	if (this->transfer_count > 0)
	{
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		GENESIS_ENGINE_ASSERT_ERROR((vkBeginCommandBuffer(this->transfer_buffer, &begin_info) == VK_SUCCESS), "failed to begin recording transfer buffer");

		for (size_t i = 0; i < this->copy_buffer_list_index; i++)
		{
			vkCmdCopyBuffer(this->transfer_buffer, this->copy_buffer_list[i].source, this->copy_buffer_list[i].destination, 1, &this->copy_buffer_list[i].region);
		}

		for (size_t i = 0; i < this->fill_texture_list_index; i++)
		{
			transitionImageLayout(this->transfer_buffer, this->fill_texture_list[i].destination, this->fill_texture_list[i].format, this->fill_texture_list[i].old_layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			copyBufferToImage(this->transfer_buffer, this->fill_texture_list[i].source, this->fill_texture_list[i].destination, this->fill_texture_list[i].size);
			transitionImageLayout(this->transfer_buffer, this->fill_texture_list[i].destination, this->fill_texture_list[i].format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, this->fill_texture_list[i].new_layout);
		}

		vkEndCommandBuffer(this->transfer_buffer);

		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &this->transfer_buffer;
		submit_info.waitSemaphoreCount = 0;
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &this->transfer_done_semaphore;

		GENESIS_ENGINE_ASSERT_ERROR((vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE) == VK_SUCCESS), "failed to submit transfer buffer");

		this->transfer_count = 0;
		submitted = true;
	}
	this->buffer_lock.unlock();

	return submitted;
}

void VulkanTransferBuffer::copyBuffer(VulkanBuffer* source, uint64_t source_offset, VulkanBuffer* destination, uint64_t destination_offset, uint64_t copy_size)
{
	this->buffer_lock.lock();

	VkBufferCopy copy_region = {};
	copy_region.srcOffset = source_offset;
	copy_region.dstOffset = destination_offset;
	copy_region.size = copy_size;

	this->copy_buffer_list[this->copy_buffer_list_index] = {source->get(), destination->get(), copy_region, destination};
	this->copy_buffer_list_index++;

	this->transfer_count++;
	this->buffer_lock.unlock();
}

void VulkanTransferBuffer::fillTexture(VulkanBuffer* source, VulkanTexture* destination)
{
	this->buffer_lock.lock();

	this->fill_texture_list[this->fill_texture_list_index] = {source->get(), destination->get(), destination->getSize(), destination->getFormat(), destination->getInitialLayout(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, destination};
	this->fill_texture_list_index++;

	this->transfer_count++;
	this->buffer_lock.unlock();
}

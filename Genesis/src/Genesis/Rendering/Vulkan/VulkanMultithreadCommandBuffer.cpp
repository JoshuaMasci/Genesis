#include "VulkanMultithreadCommandBuffer.hpp"

using namespace Genesis;

VulkanMultithreadCommandBuffer::VulkanMultithreadCommandBuffer(VulkanCommandPool* primary_command_pool, Array<VulkanCommandPool*>* secondary_command_pools)
{
	this->primary_command_pool = primary_command_pool;
	this->secondary_command_pools = secondary_command_pools;

	this->primary_command_buffer = this->primary_command_pool->getCommandBuffer();
	
	this->secondary_command_buffers = Array<VkCommandBuffer>(this->secondary_command_pools->size());
	for (int i = 0; i < this->secondary_command_buffers.size(); i++)
	{
		this->secondary_command_buffers[i] = (*this->secondary_command_pools)[i]->getCommandBuffer();
	}
}

VulkanMultithreadCommandBuffer::~VulkanMultithreadCommandBuffer()
{
	this->primary_command_pool->releaseCommandBuffer(this->primary_command_buffer);
	for (int i = 0; i < this->secondary_command_buffers.size(); i++)
	{
		(*this->secondary_command_pools)[i]->releaseCommandBuffer(this->secondary_command_buffers[i]);
	}
}

void VulkanMultithreadCommandBuffer::beginCommandBuffer(VkRenderPassBeginInfo& render_pass_info)
{
	vkResetCommandBuffer(this->primary_command_buffer, 0);

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	if (vkBeginCommandBuffer(this->primary_command_buffer, &begin_info) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	vkCmdBeginRenderPass(this->primary_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	VkCommandBufferInheritanceInfo inheritance_info = {};
	inheritance_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	inheritance_info.renderPass = render_pass_info.renderPass;
	inheritance_info.framebuffer = render_pass_info.framebuffer;

	for (int i = 0; i < this->secondary_command_buffers.size(); i++)
	{
		VkCommandBuffer buffer = this->secondary_command_buffers[i];
		vkResetCommandBuffer(buffer, 0);

		begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		begin_info.pInheritanceInfo = &inheritance_info;

		if (vkBeginCommandBuffer(buffer, &begin_info) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}
	}
}

void VulkanMultithreadCommandBuffer::endCommandBuffer()
{
	for (int i = 0; i < this->secondary_command_buffers.size(); i++)
	{
		VkCommandBuffer buffer = this->secondary_command_buffers[i];
		if (vkEndCommandBuffer(buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}
	
	vkCmdExecuteCommands(this->primary_command_buffer, (uint32_t)this->secondary_command_buffers.size(), this->secondary_command_buffers.data());
	vkCmdEndRenderPass(this->primary_command_buffer);
	vkEndCommandBuffer(this->primary_command_buffer);
}

void VulkanMultithreadCommandBuffer::submitCommandBuffer(VkQueue queue, Array<VkSemaphore>& wait_semaphores, Array<VkPipelineStageFlags>& wait_states, Array<VkSemaphore>& signal_semaphores, VkFence trigger_fence)
{
	//Submit CommandBuffers
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	submit_info.waitSemaphoreCount = (uint32_t)wait_semaphores.size();
	submit_info.pWaitSemaphores = wait_semaphores.data();
	submit_info.pWaitDstStageMask = wait_states.data();

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &this->primary_command_buffer;

	submit_info.signalSemaphoreCount = (uint32_t)signal_semaphores.size();
	submit_info.pSignalSemaphores = signal_semaphores.data();

	if (vkQueueSubmit(queue, 1, &submit_info, trigger_fence) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}
}

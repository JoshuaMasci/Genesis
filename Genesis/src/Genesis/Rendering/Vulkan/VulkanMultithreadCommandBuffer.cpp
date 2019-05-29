#include "VulkanMultithreadCommandBuffer.hpp"

using namespace Genesis;

VulkanMultithreadCommandBuffer::VulkanMultithreadCommandBuffer(VulkanDevice* device, VkCommandPool command_pool, uint32_t secondary_command_buffers)
{
	this->device = device->getDevice();
	this->command_pool = command_pool;

	//PRIMARY
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = this->command_pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	if (vkAllocateCommandBuffers(this->device, &allocInfo, &this->primary_command_buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}

	//SECONDARY
	this->secondary_command_buffers = Array<VkCommandBuffer>(secondary_command_buffers);

	VkCommandBufferAllocateInfo secondary_buffers_info = {};
	secondary_buffers_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	secondary_buffers_info.commandPool = this->command_pool;
	secondary_buffers_info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	secondary_buffers_info.commandBufferCount = (uint32_t)this->secondary_command_buffers.size();
	if (vkAllocateCommandBuffers(this->device, &secondary_buffers_info, this->secondary_command_buffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

VulkanMultithreadCommandBuffer::~VulkanMultithreadCommandBuffer()
{
	//Tells the driver we are done with these
	vkFreeCommandBuffers(this->device, this->command_pool, 1, &this->primary_command_buffer);
	vkFreeCommandBuffers(this->device, this->command_pool, (uint32_t)this->secondary_command_buffers.size(), this->secondary_command_buffers.data());
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

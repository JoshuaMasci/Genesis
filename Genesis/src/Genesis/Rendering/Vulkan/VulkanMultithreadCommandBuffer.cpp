#include "VulkanMultithreadCommandBuffer.hpp"

using namespace Genesis;

VulkanMultithreadCommandBuffer::VulkanMultithreadCommandBuffer(uint32_t frame_index, VkDevice device, VulkanCommandPool* primary_command_pool, VulkanPipelinePool* pipeline_pool, Array<VulkanThread>& threads, VulkanUniformPool* uniform_pool, VkSampler temp_sampler)
	:frame_index(frame_index)
{
	this->primary_command_pool = primary_command_pool;
	this->primary_command_buffer = this->primary_command_pool->getCommandBuffer();

	this->secondary_command_buffers.resize(threads.size());
	for (size_t i = 0; i < this->secondary_command_buffers.size(); i++)
	{
		this->secondary_command_buffers[i] = new VulkanCommandBuffer((uint32_t)i, frame_index, device, threads[i].secondary_graphics_pool, pipeline_pool, threads[i].descriptor_pool, uniform_pool, temp_sampler);
	}
}

VulkanMultithreadCommandBuffer::~VulkanMultithreadCommandBuffer()
{
	for (size_t i = 0; i < this->secondary_command_buffers.size(); i++)
	{
		delete this->secondary_command_buffers[i];
	}
	this->primary_command_pool->freeCommandBuffer(this->primary_command_buffer);
}

void VulkanMultithreadCommandBuffer::beginCommandBufferPrimary(VkRenderPassBeginInfo& render_pass_info)
{
	vkResetCommandBuffer(this->primary_command_buffer, 0);

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	if (vkBeginCommandBuffer(this->primary_command_buffer, &begin_info) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	vkCmdBeginRenderPass(this->primary_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
	//Setup Default Scissor
	vkCmdSetScissor(this->primary_command_buffer, 0, 1, &render_pass_info.renderArea);

	VkCommandBufferInheritanceInfo inheritance_info = {};
	inheritance_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	inheritance_info.renderPass = render_pass_info.renderPass;
	inheritance_info.framebuffer = render_pass_info.framebuffer;

	for (size_t i = 0; i < this->secondary_command_buffers.size(); i++)
	{
		this->secondary_command_buffers[i]->beginCommandBufferSecondary(inheritance_info, render_pass_info.renderArea.extent);
	}
}

void VulkanMultithreadCommandBuffer::endCommandBuffer()
{
	Array<VkCommandBuffer> temp_buffers(this->secondary_command_buffers.size());
	for (size_t i = 0; i < this->secondary_command_buffers.size(); i++)
	{
		this->secondary_command_buffers[i]->endCommandBuffer();
		temp_buffers[i] = this->secondary_command_buffers[i]->get();
	}
	
	vkCmdExecuteCommands(this->primary_command_buffer, (uint32_t)temp_buffers.size(), temp_buffers.data());
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

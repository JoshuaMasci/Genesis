#include "VulkanBackend.hpp"

#define VMA_IMPLEMENTATION

#include "Genesis/Rendering/Vulkan/VulkanInstance.hpp"
#include "Genesis/Rendering/Vulkan/VulkanBuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanRenderPipline.hpp"

using namespace Genesis;

VkBufferUsageFlags getBufferUsage(BufferType usage)
{
	switch (usage)
	{
	case BufferType::Uniform:
		return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; //Probobly won't be held in GPU_Only space, copy bit not needed for now
	case BufferType::Index:
		return VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT; //Likely going to need to copy into the buffer
	case BufferType::Vertex:
		return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT; //Likely going to need to copy into the buffer
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

VulkanBackend::VulkanBackend(Window* window, uint32_t number_of_threads)
{
	this->vulkan = new VulkanInstance(window, number_of_threads);
}

VulkanBackend::~VulkanBackend()
{
	delete this->vulkan;
}

void VulkanBackend::beginFrame()
{
	bool image_acquired = this->vulkan->AcquireSwapchainImage(this->swapchain_image_index);

	if (image_acquired == false)
	{
		return;
	}

	vkWaitForFences(this->vulkan->device->getDevice(), 1, &this->vulkan->command_buffer_done_fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(this->vulkan->device->getDevice(), 1, &this->vulkan->command_buffer_done_fence);

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	if (vkBeginCommandBuffer(this->vulkan->command_pool->graphics_command_buffer, &begin_info) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkExtent2D swapchain_extent = this->vulkan->swapchain->getSwapchainExtent();

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = this->vulkan->screen_render_pass;
	renderPassInfo.framebuffer = this->vulkan->swapchain_framebuffers->getSwapchainFramebuffer(this->swapchain_image_index);
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapchain_extent;

	VkClearValue clearValues[2];
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = 2;
	renderPassInfo.pClearValues = clearValues;

	vkCmdBeginRenderPass(this->vulkan->command_pool->graphics_command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	VkCommandBufferInheritanceInfo inheritance_info = {};
	inheritance_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	inheritance_info.renderPass = renderPassInfo.renderPass;
	inheritance_info.framebuffer = renderPassInfo.framebuffer;

	//TEMP STUFF
	for (int i = 0; i < this->vulkan->command_pool->graphics_secondary_command_buffers.size(); i++)
	{
		VkCommandBuffer buffer = this->vulkan->command_pool->graphics_secondary_command_buffers[i];
		vkResetCommandBuffer(buffer, 0);

		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		begin_info.pInheritanceInfo = &inheritance_info;

		if (vkBeginCommandBuffer(buffer, &begin_info) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}
	}
}

void VulkanBackend::endFrame()
{
	for (int i = 0; i < this->vulkan->command_pool->graphics_secondary_command_buffers.size(); i++)
	{
		VkCommandBuffer buffer = this->vulkan->command_pool->graphics_secondary_command_buffers[i];
		if (vkEndCommandBuffer(buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	if (this->vulkan->swapchain == nullptr)
	{
		return;
	}

	/*for (int i = 0; i < this->vulkan->command_pool->graphics_command_buffers.size(); i++)
	{
		VkCommandBuffer buffer = this->vulkan->command_pool->graphics_command_buffers[i];

		//Submit CommandBuffers
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 0;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &this->vulkan->command_pool->graphics_command_buffers[i];

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &this->vulkan->command_pool->graphics_command_buffer_finished[i];

		if (vkQueueSubmit(this->vulkan->device->getGraphicsQueue(), 1, &submitInfo, this->vulkan->command_buffer_done_fence[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}
	}*/

	vkCmdExecuteCommands(this->vulkan->command_pool->graphics_command_buffer, (uint32_t)this->vulkan->command_pool->graphics_secondary_command_buffers.size(), this->vulkan->command_pool->graphics_secondary_command_buffers.data());
	vkCmdEndRenderPass(this->vulkan->command_pool->graphics_command_buffer);
	vkEndCommandBuffer(this->vulkan->command_pool->graphics_command_buffer);

	//Submit CommandBuffers
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &this->vulkan->image_available_semaphore;
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.pWaitDstStageMask = wait_stages;

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &this->vulkan->command_pool->graphics_command_buffer;

	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &this->vulkan->command_pool->graphics_command_buffer_finished;

	if (vkQueueSubmit(this->vulkan->device->getGraphicsQueue(), 1, &submit_info, this->vulkan->command_buffer_done_fence) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	//Present the image to the screen
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &this->vulkan->command_pool->graphics_command_buffer_finished;

	VkSwapchainKHR swapChains[] = { this->vulkan->swapchain->getSwapchain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &this->swapchain_image_index;

	VkResult result = vkQueuePresentKHR(this->vulkan->device->getPresentQueue(), &presentInfo);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present image");
	}
}

Buffer* VulkanBackend::createBuffer(uint64_t size_bytes, BufferType type, MemoryUsage memory_usage)
{
	return new VulkanBuffer(this->vulkan, size_bytes, getBufferUsage(type), getMemoryUsage(memory_usage));
}

void VulkanBackend::drawMeshScreen(uint32_t thread, Buffer* vertex_buffer, Buffer* index_buffer, uint32_t index_count, matrix4F mvp)
{
	if (this->vulkan->swapchain == nullptr)
	{
		return;
	}

	VkCommandBuffer buffer = this->vulkan->command_pool->graphics_secondary_command_buffers[thread];

	vkCmdPushConstants(buffer, this->vulkan->colored_mesh_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(matrix4F), &mvp);
	vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->vulkan->colored_mesh_screen_pipeline->getPipeline());

	VkBuffer vertexBuffers[] = { ((VulkanBuffer*)vertex_buffer)->getBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(buffer, ((VulkanBuffer*)index_buffer)->getBuffer(), 0, VK_INDEX_TYPE_UINT16);

	vkCmdDrawIndexed(buffer, index_count, 1, 0, 0, 0);
}

void VulkanBackend::waitTillDone()
{
	vkDeviceWaitIdle(this->vulkan->device->getDevice());
}

#include "VulkanCommandBuffer.hpp"

#include "Genesis/Rendering/Vulkan/VulkanTexture.hpp"
#include "Genesis/Rendering/Vulkan/VulkanView.hpp"

using namespace Genesis;

VulkanCommandBuffer::VulkanCommandBuffer(uint32_t thread_index, uint32_t frame_index, VulkanCommandPool* command_pool)
	:thread_index(thread_index),
	frame_index(frame_index)
{
	this->command_pool = command_pool;
	this->command_buffer = this->command_pool->getCommandBuffer();
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
	this->command_pool->freeCommandBuffer(this->command_buffer);
}

void VulkanCommandBuffer::beginCommandBuffer(VkRenderPassBeginInfo& render_pass_info, VkSubpassContents mode)
{
	vkResetCommandBuffer(this->command_buffer, 0);

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	if (vkBeginCommandBuffer(this->command_buffer, &begin_info) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	vkCmdBeginRenderPass(this->command_buffer, &render_pass_info, mode);
}

void VulkanCommandBuffer::endCommandBuffer()
{
	vkCmdEndRenderPass(this->command_buffer);
	vkEndCommandBuffer(this->command_buffer);
}

void VulkanCommandBuffer::submitCommandBuffer(VkQueue queue, Array<VkSemaphore>& wait_semaphores, Array<VkPipelineStageFlags>& wait_states, Array<VkSemaphore>& signal_semaphores, VkFence trigger_fence)
{
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	submit_info.waitSemaphoreCount = (uint32_t)wait_semaphores.size();
	submit_info.pWaitSemaphores = wait_semaphores.data();
	submit_info.pWaitDstStageMask = wait_states.data();

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &this->command_buffer;

	submit_info.signalSemaphoreCount = (uint32_t)signal_semaphores.size();
	submit_info.pSignalSemaphores = signal_semaphores.data();

	if (vkQueueSubmit(queue, 1, &submit_info, trigger_fence) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}
}

void VulkanCommandBuffer::setShader(ShaderHandle shader)
{
	//Clear old uniforms
	this->uniforms.clear();
	this->current_shader = (VulkanShader*)shader;
}

void VulkanCommandBuffer::setPipelineSettings(PipelineSettings& settings)
{
	this->current_settings = settings;
}

void VulkanCommandBuffer::setScissor(vector2U offset, vector2U extent)
{
	VkRect2D rect = {};
	rect.offset = {(int32_t)offset.x, (int32_t)offset.y};
	rect.extent = {extent.x, extent.y};
	vkCmdSetScissor(this->command_buffer, 0, 1, &rect);
}

void VulkanCommandBuffer::setUniformFloat(string name, float value)
{
	assert(this->current_shader != nullptr);

	UniformData data;
	data.type = data.Float;
	data.data.scalar.x = value;

	this->uniforms[name] = data;
}

void VulkanCommandBuffer::setUniformVec4(string name, vector4F value)
{
	assert(this->current_shader != nullptr);

	UniformData data;
	data.type = data.Vector4;
	data.data.scalar = value;

	this->uniforms[name] = data;
}

void VulkanCommandBuffer::setUniformMat4(string name, matrix4F value)
{
	assert(this->current_shader != nullptr);

	UniformData data;
	data.type = data.Float;
	data.data.matrix = value;

	this->uniforms[name] = data;
}

void VulkanCommandBuffer::setUniformTexture(string name, TextureHandle texture)
{
	assert(this->current_shader != nullptr);

	UniformData data;
	data.type = data.Image;
	data.data.image = ((VulkanTexture*) texture)->getImageView();

	this->uniforms[name] = data;
}

void VulkanCommandBuffer::setUniformView(string name, ViewHandle view, uint16_t view_image_index)
{
	assert(this->current_shader != nullptr);

	UniformData data;
	data.type = data.Image;
	data.data.image = ((VulkanView*)view)->getFramebuffer(this->frame_index)->getImageView(view_image_index);

	this->uniforms[name] = data;
}

void VulkanCommandBuffer::drawIndexed(VertexBufferHandle vertex_handle, IndexBufferHandle index_handle)
{
	this->drawIndexedOffset(vertex_handle, index_handle, 0, ((VulkanIndexBuffer*)index_handle)->getIndicesCount());
}

void VulkanCommandBuffer::drawIndexedOffset(VertexBufferHandle vertex_handle, IndexBufferHandle index_handle, uint32_t index_offset, uint32_t index_count)
{

}

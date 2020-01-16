#include "VulkanView.hpp"

#include "Genesis/Rendering/Vulkan/VulkanShader.hpp"
#include "Genesis/Rendering/Vulkan/VulkanBuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanTexture.hpp"
#include "Genesis/Rendering/Vulkan/VulkanFramebuffer.hpp"

using namespace Genesis;

VulkanViewSingleThread::VulkanViewSingleThread(VulkanDevice* device, uint32_t frame_count, VulkanCommandPool* command_pool, VulkanThreadPipelinePool* pipeline_pool, VulkanDescriptorPool* descriptor_pool, VulkanSamplerPool* sampler_pool, List<VulkanTransferBuffer*> transfer_buffers, VkExtent2D size, List<VkFormat>& color_formats, VkFormat depth_format, VkRenderPass render_pass)
{
	this->device = device;

	this->size = size;
	this->color_formats = color_formats;
	this->depth_format = depth_format;
	this->render_pass = render_pass;

	this->clear_values.resize(color_formats.size());
	for (size_t i = 0; i < clear_values.size(); i++)
	{
		this->clear_values[i].color = { 0.f, 0.0f, 0.0f, 0.0f };
	}

	if (depth_format != VK_FORMAT_UNDEFINED)
	{
		size_t array_size = clear_values.size();
		this->clear_values.resize(array_size + 1);
		this->clear_values[array_size].depthStencil = { 1.0f, 0 };
	}

	this->frames.resize(frame_count);
	for (size_t i = 0; i < this->frames.size(); i++)
	{
		this->frames[i].framebuffer = new VulkanFramebuffer(this->device, this->size, this->color_formats, this->depth_format, this->render_pass);
		this->frames[i].command_buffer = new VulkanCommandBufferSingle(this->device, command_pool, pipeline_pool, descriptor_pool, sampler_pool, transfer_buffers[i], (uint32_t)i);
	}
}

VulkanViewSingleThread::~VulkanViewSingleThread()
{
	for (size_t i = 0; i < this->frames.size(); i++)
	{
		delete this->frames[i].framebuffer;
		delete this->frames[i].command_buffer;
	}
}

void VulkanViewSingleThread::start(uint32_t frame_index)
{
	this->frame_index = frame_index;

	VkExtent2D old_size = this->frames[this->frame_index].framebuffer->getSize();
	if (old_size.width != this->size.width && old_size.height != this->size.height)
	{
		delete this->frames[this->frame_index].framebuffer;
		this->frames[this->frame_index].framebuffer = new VulkanFramebuffer(this->device, this->size, this->color_formats, this->depth_format, this->render_pass);
	}

	VkRect2D rect = {};
	rect.offset = { 0, 0 };
	rect.extent = this->frames[this->frame_index].framebuffer->getSize();
	this->frames[this->frame_index].command_buffer->start(this->frames[this->frame_index].framebuffer->get(), this->render_pass, rect, this->clear_values, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanViewSingleThread::end()
{
	this->frames[this->frame_index].command_buffer->end();
}

/*void VulkanViewSingleThread::submit(VkQueue queue)
{
	List<VkSemaphore> signal_semaphores(1);
	signal_semaphores[0] = this->frames[this->frame_index].command_buffer_semaphore;

	this->frames[this->frame_index].command_buffer->submit(queue, List<VkSemaphore>(), List<VkPipelineStageFlags>(), signal_semaphores, VK_NULL_HANDLE);
}*/

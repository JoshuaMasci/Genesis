#include "VulkanView.hpp"

using namespace Genesis;

VulkanView::VulkanView(VulkanDevice* device, VulkanAllocator* allocator, uint32_t frames_in_flight, VkExtent2D size, Array<VkFormat>& color_formats, VkFormat depth_format, VkRenderPass render_pass, Array<VulkanCommandBuffer*> command_buffers)
{
	this->device = device;
	this->allocator = allocator;

	this->size = size;
	this->color_formats = color_formats;
	this->depth_format = depth_format;
	this->render_pass = render_pass;

	this->command_buffers.resize(frames_in_flight);
	for (size_t i = 0; i < this->command_buffers.size(); i++)
	{
		this->command_buffers[i] = command_buffers[i];
	}

	this->framebuffers.resize(frames_in_flight);
	for (size_t i = 0; i < this->framebuffers.size(); i++)
	{
		this->framebuffers[i] = new VulkanFramebuffer(this->device->get(), this->allocator, this->size, this->color_formats, this->depth_format, this->render_pass);
	}

	this->view_done_semaphores.resize(frames_in_flight);
	for (size_t i = 0; i < this->view_done_semaphores.size(); i++)
	{
		this->view_done_semaphores[i] = this->device->createSemaphore();
	}
}

VulkanView::~VulkanView()
{
	for (size_t i = 0; i < this->view_done_semaphores.size(); i++)
	{
		this->device->destroySemaphore(this->view_done_semaphores[i]);
	}

	for (size_t i = 0; i < this->framebuffers.size(); i++)
	{
		delete this->framebuffers[i];
	}

	for (size_t i = 0; i < this->command_buffers.size(); i++)
	{
		delete this->command_buffers[i];
	}
}

void VulkanView::startView(uint32_t frame)
{
	VulkanFramebuffer* framebuffer = this->framebuffers[frame];

	if (framebuffer->getSize().width != this->size.width || framebuffer->getSize().height != this->size.height)
	{
		delete framebuffer;
		this->framebuffers[frame] = new VulkanFramebuffer(this->device->get(), this->allocator, this->size, this->color_formats, this->depth_format, this->render_pass);
		framebuffer = this->framebuffers[frame];
	}

	VulkanCommandBuffer* command_buffer = this->command_buffers[frame];

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = framebuffer->getRenderPass();
	renderPassInfo.framebuffer = framebuffer->get();
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = framebuffer->getSize();

	renderPassInfo.clearValueCount = (uint32_t) this->clear_values.size();
	renderPassInfo.pClearValues = this->clear_values.data();

	command_buffer->beginCommandBufferPrimary(renderPassInfo);
}

void VulkanView::endView(uint32_t frame)
{
	this->command_buffers[frame]->endCommandBuffer();
}

void VulkanView::submitView(uint32_t frame, vector<VulkanView*> sub_views, VkFence view_done_fence)
{
	Array<VkSemaphore> wait_semaphores(sub_views.size());
	Array<VkPipelineStageFlags> wait_states(sub_views.size());

	for (size_t i = 0; i < wait_semaphores.size(); i++)
	{
		wait_semaphores[i] = sub_views[i]->getWaitSemaphore(frame);
		wait_states[i] = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; //TODO figure out better system maybe
	}

	Array<VkSemaphore> signal_semaphores(1);
	signal_semaphores[0] = this->view_done_semaphores[frame];

	this->command_buffers[frame]->submitCommandBuffer(this->device->getGraphicsQueue(), wait_semaphores, wait_states, signal_semaphores, view_done_fence);
}

void VulkanView::setViewSize(VkExtent2D size)
{
	this->size = size;
}

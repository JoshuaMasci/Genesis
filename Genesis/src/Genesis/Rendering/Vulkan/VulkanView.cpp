#include "VulkanView.hpp"

using namespace Genesis;

VulkanView::VulkanView(VulkanDevice* device, VulkanAllocator* allocator, VulkanMultithreadCommandBuffer* command_buffer, VkExtent2D size, VulkanFramebufferLayout* layout)
{
	this->device = device;

	this->command_buffer = command_buffer;

	this->framebuffer = layout->createFramebuffer(size, allocator);

	this->view_done_semaphore = this->device->createSemaphore();
}

VulkanView::~VulkanView()
{
	this->device->destroySemaphore(this->view_done_semaphore);

	delete this->framebuffer;
	delete this->command_buffer;
}

void VulkanView::startView()
{
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = this->framebuffer->getRenderPass();
	renderPassInfo.framebuffer = this->framebuffer->get();
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = this->framebuffer->getSize();

	//TODO clear values that line up with renderpass
	VkClearValue clearValues[2];
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = 2;
	renderPassInfo.pClearValues = clearValues;

	this->command_buffer->beginCommandBuffer(renderPassInfo);
}

void VulkanView::endView()
{
	this->command_buffer->endCommandBuffer();
}

void VulkanView::submitView(vector<VulkanView*> sub_views, VkFence view_done_fence)
{
	Array<VkSemaphore> wait_semaphores(sub_views.size());
	Array<VkPipelineStageFlags> wait_states(sub_views.size());

	for (size_t i = 0; i < wait_semaphores.size(); i++)
	{
		wait_semaphores[i] = sub_views[i]->getWaitSemaphore();
		wait_states[i] = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; //TODO figure out better system maybe
	}

	Array<VkSemaphore> signal_semaphores(1);
	signal_semaphores[0] = this->view_done_semaphore;

	this->command_buffer->submitCommandBuffer(this->device->getGraphicsQueue(), wait_semaphores, wait_states, signal_semaphores, view_done_fence);
}

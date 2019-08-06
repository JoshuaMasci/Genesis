#include "VulkanBackend.hpp"

#include "Genesis/Rendering/Vulkan/VulkanInstance.hpp"
#include "Genesis/Rendering/Vulkan/VulkanPipline.hpp"
#include "Genesis/Rendering/Vulkan/VulkanView.hpp"

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Core/MurmurHash2.hpp"

using namespace Genesis;

VkBufferUsageFlags getBufferUsage(BufferType usage)
{
	switch (usage)
	{
	case BufferType::Uniform:
		return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; //Probably won't be held in GPU_Only space, copy bit not needed for now
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

	/*VkRenderPass renderpass = this->vulkan->screen_layout->getRenderPass();
	PipelineSettings settings;
	VertexInputDescription vertex_description
	({
		{"in_position", VertexElementType::float_3},
		{"in_normal", VertexElementType::float_3},
		{"in_uv", VertexElementType::float_2}
		});

	VkExtent2D extent = this->vulkan->swapchain->getSwapchainExtent();


	MurmurHash2 murmur_hash;
	murmur_hash.begin();
	murmur_hash.add(renderpass);
	murmur_hash.add(settings.getHash());
	murmur_hash.add(extent);
	murmur_hash.add(vertex_description.getHash());
	uint32_t pipeline_hash = murmur_hash.end();

	printf("pipeline_hash: %d\n", pipeline_hash);*/
}

VulkanBackend::~VulkanBackend()
{
	delete this->vulkan;
}

bool VulkanBackend::beginFrame()
{
	VulkanFrame* frame = &this->vulkan->frames_in_flight[this->frame_index];
	bool image_acquired = this->vulkan->acquireSwapchainImage(this->swapchain_image_index, frame->image_available_semaphore);

	if (image_acquired == false)
	{
		return false;
	}

	vkWaitForFences(this->vulkan->device->get(), 1, &frame->command_buffer_done_fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(this->vulkan->device->get(), 1, &frame->command_buffer_done_fence);

	VkRenderPassBeginInfo renderPassInfo = {};
	{
		VkExtent2D swapchain_extent = this->vulkan->swapchain->getSwapchainExtent();
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = this->vulkan->swapchain_framebuffers->getRenderPass();
		renderPassInfo.framebuffer = this->vulkan->swapchain_framebuffers->getSwapchainFramebuffer(this->swapchain_image_index);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain_extent;
		VkClearValue clearValues[2];
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues;
	}
	frame->command_buffer->beginCommandBuffer(renderPassInfo);

	return true;
}

void VulkanBackend::endFrame()
{
	if (this->vulkan->swapchain == nullptr)
	{
		return;
	}

	VulkanFrame* frame = &this->vulkan->frames_in_flight[this->frame_index];

	frame->command_buffer->endCommandBuffer();
}

void VulkanBackend::submitFrame(vector<ViewHandle> sub_views)
{
	if (this->vulkan->swapchain == nullptr)
	{
		return;
	}

	VulkanFrame* frame = &this->vulkan->frames_in_flight[this->frame_index];

	size_t wait_stages = sub_views.size() + 1;

	Array<VkSemaphore> wait_semaphores(wait_stages);
	wait_semaphores[0] = frame->image_available_semaphore;
	Array<VkPipelineStageFlags> wait_states(wait_stages);
	wait_states[0] = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

	for (size_t i = 1; i < wait_stages; i++)
	{
		wait_semaphores[i] = ((VulkanView*)sub_views[i - 1])->getWaitSemaphore(this->frame_index);
		wait_states[i] = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}

	Array<VkSemaphore> signal_semaphores(1);
	signal_semaphores[0] = frame->command_buffer_done_semaphore;

	frame->command_buffer->submitCommandBuffer(this->vulkan->device->getGraphicsQueue(), wait_semaphores, wait_states, signal_semaphores, frame->command_buffer_done_fence);

	//Present the image to the screen
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &frame->command_buffer_done_semaphore;

	VkSwapchainKHR swapChains[] = { this->vulkan->swapchain->get() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &this->swapchain_image_index;

	VkResult result = vkQueuePresentKHR(this->vulkan->device->getPresentQueue(), &presentInfo);

	this->frame_index = (this->frame_index + 1) % this->vulkan->frames_in_flight.size();

	//Cleanup Resources
	this->vulkan->cycleResourceDeleters();
}

ShaderHandle VulkanBackend::createShader(string vert_data, string frag_data)
{
	VulkanShader* shader = new VulkanShader(this->vulkan->device->get(), vert_data, frag_data, this->vulkan->descriptor_layouts);
	return shader;
}

void VulkanBackend::destroyShader(ShaderHandle shader_handle)
{
	//TEMP
	delete (VulkanShader*)shader_handle;
}

BufferHandle VulkanBackend::createBuffer(uint64_t size_bytes, BufferType type, MemoryUsage memory_usage)
{
	VulkanBuffer* buffer = new VulkanBuffer(this->vulkan->allocator, size_bytes, getBufferUsage(type), getMemoryUsage(memory_usage));
	return (BufferHandle)buffer;
}

void VulkanBackend::fillBuffer(BufferHandle buffer_handle, void* data, uint64_t data_size)
{
	VulkanBuffer* buffer = (VulkanBuffer*)buffer_handle;
	buffer->fillBuffer(this->vulkan->graphics_command_pool_set->getPrimaryCommandPool(), this->vulkan->device->getGraphicsQueue(), data, data_size);
}

void VulkanBackend::destroyBuffer(BufferHandle buffer_handle)
{
	this->vulkan->buffer_deleter->addToQueue((VulkanBuffer*)buffer_handle);
}

TextureHandle VulkanBackend::createTexture(vector2U size)
{
	VulkanTexture* texture = new VulkanTexture(this->vulkan->device, this->vulkan->allocator, {size.x, size.y}, getMemoryUsage(MemoryUsage::GPU_Only), this->vulkan->linear_sampler);
	return (TextureHandle)texture;
}

void VulkanBackend::fillTexture(TextureHandle texture_handle, void* data, uint64_t data_size)
{
	VulkanTexture* texture = (VulkanTexture*)texture_handle;
	texture->fillTexture(this->vulkan->graphics_command_pool_set->getPrimaryCommandPool(), this->vulkan->device->getGraphicsQueue(), data, data_size);
}

void VulkanBackend::destroyTexture(TextureHandle texture_handle)
{
	this->vulkan->texture_deleter->addToQueue((VulkanTexture*)texture_handle);
}

ViewHandle VulkanBackend::createView(ViewType type, vector2U size)
{
	VkExtent2D vk_size = {size.x, size.y};

	uint32_t frames_in_flight = (uint32_t)this->vulkan->frames_in_flight.size();

	VulkanFramebufferLayout* layout = nullptr;
	if (type == ViewType::FrameBuffer)
	{
		layout = this->vulkan->color_pass_layout;
	}
	else if (type == ViewType::ShadowMap)
	{
		layout = this->vulkan->shadow_pass_layout;
	}

	VulkanView* view = new VulkanView(this->vulkan->device, this->vulkan->allocator, frames_in_flight, this->vulkan->graphics_command_pool_set, vk_size, layout);

	Array<VkClearValue> clear_values(2);
	clear_values[0].color = { 0.8f, 0.0f, 0.8f, 1.0f };
	clear_values[1].depthStencil = { 1.0f, 0 };
	view->setClearValues(clear_values);

	return (ViewHandle)view;
}

void VulkanBackend::destroyView(ViewHandle index)
{
	this->vulkan->view_deleter->addToQueue((VulkanView*)index);
}

void VulkanBackend::startView(ViewHandle index)
{
	VulkanView* view = (VulkanView*)index;
	view->startView(this->frame_index);
}

void VulkanBackend::endView(ViewHandle index)
{
	VulkanView* view = (VulkanView*)index;
	view->endView(this->frame_index);
}

void VulkanBackend::sumbitView(ViewHandle index)
{
	VulkanView* view = (VulkanView*)index;
	view->submitView(this->frame_index, vector<VulkanView*>(), VK_NULL_HANDLE);
}

matrix4F VulkanBackend::getPerspectiveMatrix(Camera* camera, float aspect_ratio)
{
	float fov = 1.0f / tan(glm::radians(camera->frame_of_view) / 2.0f);
	matrix4F proj = glm::infinitePerspective(fov, aspect_ratio, camera->z_near);
	proj[1][1] *= -1; //Need to apply this because vulkan flips the y-axis and that's not what I need

	return proj;
}

matrix4F VulkanBackend::getPerspectiveMatrix(Camera* camera, ViewHandle view_handle)
{
	VulkanView* view = (VulkanView*)view_handle;

	VkExtent2D screen_size = view->getViewSize();
	float aspect_ratio = ((float)screen_size.width) / ((float)screen_size.height);
	return this->getPerspectiveMatrix(camera, aspect_ratio);
}

vector2U VulkanBackend::getScreenSize()
{
	VkExtent2D extent = this->vulkan->swapchain->getSwapchainExtent();
	return { extent.width, extent.height };
}

void VulkanBackend::waitTillDone()
{
	vkDeviceWaitIdle(this->vulkan->device->get());
}

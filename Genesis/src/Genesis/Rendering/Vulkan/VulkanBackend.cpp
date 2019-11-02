#include "VulkanBackend.hpp"

#include "Genesis/Rendering/Vulkan/VulkanInstance.hpp"
#include "Genesis/Rendering/Vulkan/VulkanPipline.hpp"
//#include "Genesis/Rendering/Vulkan/VulkanView.hpp"

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Core/MurmurHash2.hpp"

using namespace Genesis;

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

VkFormat getImageFormat(ImageFormat format)
{
	switch (format)
	{
	case ImageFormat::RGBA_8_Unorm:
		return VK_FORMAT_R8G8B8A8_UNORM;
	case ImageFormat::R_16_Float:
		return VK_FORMAT_R16_SFLOAT;
	case ImageFormat::RG_16_Float:
		return VK_FORMAT_R16G16_SFLOAT;
	case ImageFormat::RGB_16_Float:
		return VK_FORMAT_R16G16B16_SFLOAT;
	case ImageFormat::RGBA_16_Float:
		return VK_FORMAT_R16G16B16A16_SFLOAT;
	case ImageFormat::R_32_Float:
		return VK_FORMAT_R32_SFLOAT;
	case ImageFormat::RG_32_Float:
		return VK_FORMAT_R32G32_SFLOAT;
	case ImageFormat::RGB_32_Float:
		return VK_FORMAT_R32G32B32_SFLOAT;
	case ImageFormat::RGBA_32_Float:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	case ImageFormat::D_16_Unorm:
		return VK_FORMAT_D16_UNORM;
	case ImageFormat::D_32_Float:
		return VK_FORMAT_D32_SFLOAT;
	}
	return VK_FORMAT_UNDEFINED;
}

VulkanBackend::VulkanBackend(Window* window, uint32_t number_of_threads)
{
	this->vulkan = new VulkanInstance(window, number_of_threads, 3);
}

VulkanBackend::~VulkanBackend()
{
	if (this->screen_quad_vertex != nullptr)
	{
		this->destroyVertexBuffer(this->screen_quad_vertex);
	}

	if (this->screen_quad_index != nullptr)
	{
		this->destroyIndexBuffer(this->screen_quad_index);
	}

	delete this->vulkan;
}

void VulkanBackend::setScreenSize(vector2U size)
{
}

vector2U VulkanBackend::getScreenSize()
{
	return this->vulkan->window->getWindowSize();
}

bool VulkanBackend::beginFrame()
{
	VulkanFrame* frame = &this->vulkan->frames_in_flight[this->frame_index];
	this->vulkan->acquireSwapchainImage(this->swapchain_image_index, frame->image_available_semaphore);

	if (this->vulkan->swapchain->invalid())
	{
		return false;
	}

	vkWaitForFences(this->vulkan->device->get(), 1, &frame->command_buffer_done_fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(this->vulkan->device->get(), 1, &frame->command_buffer_done_fence);

	for (uint32_t i = 0; i < (uint32_t)this->vulkan->threads.size(); i++)
	{
		this->vulkan->threads[i].descriptor_pool->resetFrame(this->frame_index);
	}

	this->vulkan->descriptor_pool->resetFrame(this->frame_index);
	this->vulkan->uniform_pool->resetFrame(this->frame_index);

	VkRenderPassBeginInfo renderPassInfo = {};
	{
		VkExtent2D swapchain_extent = this->vulkan->swapchain->getSwapchainExtent();
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = this->vulkan->swapchain->getRenderPass();
		renderPassInfo.framebuffer = this->vulkan->swapchain->getFramebuffer(this->swapchain_image_index);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain_extent;
		VkClearValue clearValues[1];
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = clearValues;
	}

	frame->command_buffer->beginCommandBufferPrimary(renderPassInfo);

	return true;
}

void VulkanBackend::endFrame()
{
	if (this->vulkan->swapchain->invalid())
	{
		return;
	}

	VulkanFrame* frame = &this->vulkan->frames_in_flight[this->frame_index];

	frame->command_buffer->endCommandBuffer();
}

void VulkanBackend::submitFrame(vector<View> sub_views)
{
	if (this->vulkan->swapchain->invalid())
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

	this->frame_index = (this->frame_index + 1) % this->vulkan->FRAME_COUNT;

	//Cleanup Resources
	this->vulkan->cycleResourceDeleters();
	this->vulkan->pipeline_pool->update();
}

VertexBuffer VulkanBackend::createVertexBuffer(void* data, uint64_t data_size, VertexInputDescription& vertex_input_description, MemoryUsage memory_usage)
{
	return (VertexBuffer) new VulkanVertexBuffer(this->vulkan->allocator, this->vulkan->primary_graphics_pool, this->vulkan->device->getGraphicsQueue(), data, data_size, getMemoryUsage(memory_usage), vertex_input_description);
}

void VulkanBackend::destroyVertexBuffer(VertexBuffer vertex_buffer_index)
{
	this->vulkan->buffer_deleter->addToQueue((VulkanBuffer*)vertex_buffer_index);
}

IndexBuffer VulkanBackend::createIndexBuffer(void* indices, uint32_t indices_count, IndexType type, MemoryUsage memory_usage)
{
	return (IndexBuffer) new VulkanIndexBuffer(this->vulkan->allocator, this->vulkan->primary_graphics_pool, this->vulkan->device->getGraphicsQueue(), indices, (indices_count * (type == IndexType::uint16 ? sizeof(uint16_t) : sizeof(uint32_t))), getMemoryUsage(memory_usage), indices_count, type);
}

void VulkanBackend::destroyIndexBuffer(IndexBuffer index_buffer_index)
{
	this->vulkan->buffer_deleter->addToQueue((VulkanBuffer*)index_buffer_index);
}

Texture VulkanBackend::createTexture(vector2U size, void* data, uint64_t data_size)
{
	VulkanTexture* texture = new VulkanTexture(this->vulkan->device, this->vulkan->allocator, {size.x, size.y}, getMemoryUsage(MemoryUsage::GPU_Only), this->vulkan->texture_sampler);
	texture->fillTexture(this->vulkan->primary_graphics_pool, this->vulkan->device->getGraphicsQueue(), data, data_size);
	return (Texture)texture;
}

void VulkanBackend::destroyTexture(Texture texture_handle)
{
	this->vulkan->texture_deleter->addToQueue((VulkanTexture*)texture_handle);
}

Shader VulkanBackend::createShader(string& vert_data, string& frag_data)
{
	VulkanShader* shader = new VulkanShader(this->vulkan->device->get(), vert_data, frag_data);
	return shader;
}

void VulkanBackend::destroyShader(Shader shader_handle)
{
	this->vulkan->shader_deleter->addToQueue((VulkanShader*)shader_handle);
}

View VulkanBackend::createView(vector2U size, FramebufferLayout& layout, CommandBufferType type)
{
	uint32_t frames_in_flight = (uint32_t)this->vulkan->FRAME_COUNT;
	VkExtent2D vk_size = {size.x, size.y};

	Array<VkFormat> color(layout.getColorCount());
	Array<VkClearValue> clear_colors(color.size());
	for (size_t i = 0; i < color.size(); i++)
	{
		color[i] = getImageFormat(layout.getColor(i));
		clear_colors[i].color = { 0.f, 0.0f, 0.0f, 0.0f };
	}

	VkFormat depth = getImageFormat(layout.getDepth());
	if (layout.getDepth() != ImageFormat::Invalid)
	{
		size_t array_size = clear_colors.size();
		clear_colors.resize(array_size + 1);
		clear_colors[array_size].depthStencil = { 1.0f, 0 };
	}

	VkRenderPass render_pass = this->vulkan->render_pass_manager->getRenderPass(layout.getHash(), color, depth);
	Array<VulkanCommandBuffer*> command_buffers(frames_in_flight);
	for (uint32_t i = 0; i < command_buffers.size(); i++)
	{
		command_buffers[i] = new VulkanCommandBuffer(0, i, this->vulkan->device->get(), this->vulkan->primary_graphics_pool, this->vulkan->pipeline_pool, this->vulkan->threads[0].descriptor_pool, this->vulkan->uniform_pool, this->vulkan->texture_sampler);
	}

	VulkanView* view = new VulkanView(this->vulkan->device, this->vulkan->allocator, frames_in_flight, vk_size, color, depth, render_pass, command_buffers, this->vulkan->view_sampler);
	view->setClearValues(clear_colors);

	return (View)view;
}

void VulkanBackend::destroyView(View index)
{
	this->vulkan->view_deleter->addToQueue((VulkanView*)index);
}

void VulkanBackend::resizeView(View index, vector2U new_size)
{
	VulkanView* view = (VulkanView*)index;
	view->setViewSize({ new_size.x, new_size.y });
}

void VulkanBackend::startView(View index)
{
	VulkanView* view = (VulkanView*)index;
	view->startView(this->frame_index);
}

void VulkanBackend::endView(View index)
{
	VulkanView* view = (VulkanView*)index;
	view->endView(this->frame_index);
}

void VulkanBackend::submitView(View index, vector<View> sub_views)
{
	vector<VulkanView*> vk_sub_views(sub_views.size());
	for (size_t i = 0; i < vk_sub_views.size(); i++)
	{
		vk_sub_views[i] = (VulkanView*)sub_views[i];
	}

	VulkanView* view = (VulkanView*)index;
	view->submitView(this->frame_index, vk_sub_views, VK_NULL_HANDLE);
}

CommandBuffer* VulkanBackend::getViewCommandBuffer(View index)
{
	return ((VulkanView*)index)->getCommandBuffer(this->frame_index);
}

CommandBuffer* VulkanBackend::getScreenCommandBuffer()
{
	if (this->vulkan->swapchain->invalid())
	{
		return (CommandBuffer*)nullptr;
	}

	return (CommandBuffer*)this->vulkan->frames_in_flight[this->frame_index].command_buffer;
}

matrix4F VulkanBackend::getPerspectiveMatrix(float fov, float z_near, float aspect_ratio)
{
	float fovy = atan(tan(glm::radians(fov) / 2.0f) / aspect_ratio) * 2.0f;
	matrix4F proj = glm::infinitePerspective(fovy, aspect_ratio, z_near);
	proj[1][1] *= -1; //Need to apply this because vulkan flips the y-axis and that's not what I need

	return proj;
}

matrix4F VulkanBackend::getPerspectiveMatrix(Camera* camera, View view_handle)
{
	VulkanView* view = (VulkanView*)view_handle;
	VkExtent2D screen_size = view->getViewSize();
	return this->getPerspectiveMatrix(camera->frame_of_view, camera->z_near, ((float)screen_size.width) / ((float)screen_size.height));
}

VertexBuffer VulkanBackend::getWholeScreenQuadVertex()
{
	if (this->screen_quad_vertex == nullptr)
	{
		vector2F vertex_data[] =
		{
			//Pos        UV
			{-1.0f, -1.0f}, {0.0f, 0.0f},
			{-1.0f, 1.0f}, {0.0f, 1.0f},
			{1.0f, -1.0f}, {1.0f, 0.0f},
			{1.0f, 1.0f}, {1.0f, 1.0f},
		};

		VertexInputDescription vi({ {"pos", VertexElementType::float_2},
									{"uv", VertexElementType::float_2} });

		this->screen_quad_vertex = this->createVertexBuffer(vertex_data, sizeof(vector2F) * 8, vi, MemoryUsage::GPU_Only);
	}

	return this->screen_quad_vertex;
}

IndexBuffer VulkanBackend::getWholeScreenQuadIndex()
{
	if (this->screen_quad_index == nullptr)
	{
		uint16_t index_data[] =
		{
			0, 1, 2, //First Tri
			3, 2, 1 // Second Tri
		};

		this->screen_quad_index = this->createIndexBuffer(index_data, sizeof(uint16_t) * 6, IndexType::uint16, MemoryUsage::GPU_Only);
	}

	return this->screen_quad_index;
}

void VulkanBackend::waitTillDone()
{
	vkDeviceWaitIdle(this->vulkan->device->get());
}

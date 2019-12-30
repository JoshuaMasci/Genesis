#include "VulkanBackend.hpp"

#include "Genesis/Rendering/Vulkan/VulkanPhysicalDevicePicker.hpp"

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
	:FRAME_COUNT(3),
	THREAD_COUNT(number_of_threads)
{
	this->window = window;

	vector<const char*> extensions;
	extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#ifdef VK_USE_PLATFORM_WIN32_KHR
	extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

	vector<const char*> layers;
	layers.push_back("VK_LAYER_LUNARG_standard_validation");
	//layers.push_back("VK_LAYER_RENDERDOC_Capture");

	this->instance = VulkanInstance::create(VK_API_VERSION_1_1, "Sandbox", VK_MAKE_VERSION(0, 0, 0), "Genesis_Engine", VK_MAKE_VERSION(0, 0, 0), extensions, layers);

	this->debug_layer = new VulkanDebugLayer(this->instance);

	this->surface = VulkanSurface::create(this->instance, this->window);

	vector<const char*> device_extensions;
	device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	this->device = new VulkanDevice(VulkanPhysicalDevicePicker::pickDevice(this->instance, this->surface), this->surface, device_extensions, layers);

	vector2U window_size = this->window->getWindowSize();
	this->swapchain = new VulkanSwapchain(this->device, { window_size.x, window_size.y }, this->surface);

	//Graphics Command Pools
	this->primary_graphics_pool = new VulkanCommandPool(this->device->get(), this->device->getGraphicsFamilyIndex(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	this->secondary_graphics_pools.resize(this->THREAD_COUNT);
	for (size_t i = 0; i < this->secondary_graphics_pools.size(); i++)
	{
		this->secondary_graphics_pools[i] = new VulkanCommandPool(this->device->get(), this->device->getGraphicsFamilyIndex(), VK_COMMAND_BUFFER_LEVEL_SECONDARY, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	}

	//Transfer Command Pool
	this->transfer_pool = new VulkanCommandPool(this->device->get(), this->device->getTransferFamilyIndex(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	this->frames.resize(this->FRAME_COUNT);
	for (size_t i = 0; i < this->frames.size(); i++)
	{
		this->frames[i].screen_command_buffer = this->primary_graphics_pool->getCommandBuffer();

		this->frames[i].image_ready_semaphore = this->device->createSemaphore();
		this->frames[i].command_buffer_done_semaphore = this->device->createSemaphore();
		this->frames[i].frame_done_fence = this->device->createFence();

		this->frames[i].transfer_buffer = new VulkanTransferBuffer(this->device, this->transfer_pool);
	}

	//Descriptor Pools
	this->descriptor_pools.resize(this->THREAD_COUNT);
	for (size_t i = 0; i < this->descriptor_pools.size(); i++)
	{
		this->descriptor_pools[i] = 
			new VulkanDescriptorPool( this->device->get(), this->FRAME_COUNT, 800000,
			{
				{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 8000},
				{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 8000}
			});
	}

	this->layout_pool = new VulkanLayoutPool(this->device->get());

	this->pipeline_pool = new VulkanPipelinePool(this->device->get());
	this->thread_pipeline_pools.resize(this->THREAD_COUNT);
	for (size_t i = 0; i < this->thread_pipeline_pools.size(); i++)
	{
		this->thread_pipeline_pools[i] = new VulkanThreadPipelinePool(this->device->get(), this->pipeline_pool);
	}

	this->render_pass_pool = new VulkanRenderPassPool(this->device->get());

	const uint8_t delay_cycles = (uint8_t)this->FRAME_COUNT + 1;
	this->buffer_deleter = new DelayedResourceDeleter<VulkanBuffer>(delay_cycles);
	this->uniform_deleter = new DelayedResourceDeleter<VulkanUniformBuffer>(delay_cycles);
	this->texture_deleter = new DelayedResourceDeleter<VulkanTexture>(delay_cycles);
	this->shader_deleter = new DelayedResourceDeleter<VulkanShader>(delay_cycles);
	//this->framebuffer_set_deleter = new DelayedResourceDeleter<VulkanFramebufferSet>(delay_cycles);
	//this->single_command_buffer_deleter = new DelayedResourceDeleter<VulkanCommandBufferSingle>(delay_cycles);
	this->view_deleter = new DelayedResourceDeleter<VulkanViewSingleThread>(delay_cycles);
}

VulkanBackend::~VulkanBackend()
{
	delete this->buffer_deleter;
	delete this->uniform_deleter;
	delete this->texture_deleter;
	delete this->shader_deleter;
	//delete this->framebuffer_set_deleter;
	//delete this->single_command_buffer_deleter;
	delete this->view_deleter;

	delete this->render_pass_pool;

	for (size_t i = 0; i < this->thread_pipeline_pools.size(); i++)
	{
		delete this->thread_pipeline_pools[i];
	}
	delete this->pipeline_pool;

	delete this->layout_pool;

	for (size_t i = 0; i < this->descriptor_pools.size(); i++)
	{
		delete this->descriptor_pools[i];
	}

	for (size_t i = 0; i < this->frames.size(); i++)
	{
		this->primary_graphics_pool->freeCommandBuffer(this->frames[i].screen_command_buffer);

		this->device->destroySemaphore(this->frames[i].image_ready_semaphore);
		this->device->destroySemaphore(this->frames[i].command_buffer_done_semaphore);
		this->device->destroyFence(this->frames[i].frame_done_fence);

		delete this->frames[i].transfer_buffer;
	}

	delete this->transfer_pool;

	for (size_t i = 0; i < this->secondary_graphics_pools.size(); i++)
	{
		delete this->secondary_graphics_pools[i];
	}
	delete this->primary_graphics_pool;

	delete this->swapchain;

	delete this->device;

	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);

	delete this->debug_layer;

	vkDestroyInstance(this->instance, nullptr);
}

void VulkanBackend::setScreenSize(vector2U size)
{

}

vector2U VulkanBackend::getScreenSize()
{
	return vector2U();
}

bool VulkanBackend::beginFrame()
{
	if (this->swapchain->invalid())
	{
		vector2U surface_size = this->window->getWindowSize();
		this->swapchain->recreateSwapchain({ surface_size.x, surface_size.y });
	}

	uint32_t index = this->swapchain->getNextImage(this->frames[this->frame_index].image_ready_semaphore);

	if (index == std::numeric_limits<uint32_t>::max())
	{
		return false;
	}

	this->swapchain_image_index = index;

	vkWaitForFences(this->device->get(), 1, &this->frames[this->frame_index].frame_done_fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(this->device->get(), 1, &this->frames[this->frame_index].frame_done_fence);

	vkResetCommandBuffer(this->frames[this->frame_index].screen_command_buffer, 0);

	this->frames[this->frame_index].transfer_buffer->reset();

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	if (vkBeginCommandBuffer(this->frames[this->frame_index].screen_command_buffer, &begin_info) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording primary command buffer!");
	}

	{
		VkRenderPassBeginInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = this->swapchain->getRenderPass();
		render_pass_info.framebuffer = this->swapchain->getFramebuffer(this->swapchain_image_index);
		render_pass_info.renderArea.offset = { 0, 0 };
		render_pass_info.renderArea.extent = this->swapchain->getSwapchainExtent();
		VkClearValue clearValues[1];
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = clearValues;

		vkCmdBeginRenderPass(this->frames[this->frame_index].screen_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
	}

	return true;
}

void VulkanBackend::endFrame()
{
	bool transfer_required = this->frames[this->frame_index].transfer_buffer->SubmitTransfers(this->device->getTransferQueue());

	vkCmdEndRenderPass(this->frames[this->frame_index].screen_command_buffer);
	vkEndCommandBuffer(this->frames[this->frame_index].screen_command_buffer);

	vector<VkSemaphore> wait_semaphores;
	vector<VkPipelineStageFlags> wait_states;

	wait_semaphores.push_back(this->frames[this->frame_index].image_ready_semaphore);
	wait_states.push_back(VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

	if (transfer_required == true)
	{
		wait_semaphores.push_back(this->frames[this->frame_index].transfer_buffer->getTransferDoneSemaphore());
		wait_states.push_back(VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT);
	}

	{
		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submit_info.waitSemaphoreCount = (uint32_t)wait_semaphores.size();
		submit_info.pWaitSemaphores = wait_semaphores.data();
		submit_info.pWaitDstStageMask = wait_states.data();

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &this->frames[this->frame_index].screen_command_buffer;

		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &this->frames[this->frame_index].command_buffer_done_semaphore;

		if (vkQueueSubmit(this->device->getGraphicsQueue(), 1, &submit_info, this->frames[this->frame_index].frame_done_fence) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit command buffer!");
		}
	}

	//Present the image to the screen
	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &this->frames[this->frame_index].command_buffer_done_semaphore;

	VkSwapchainKHR swapchains[] = { this->swapchain->get() };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;

	present_info.pImageIndices = &this->swapchain_image_index;

	VkResult result = vkQueuePresentKHR(this->device->getPresentQueue(), &present_info);

	this->frame_index = (this->frame_index + 1) % this->FRAME_COUNT;

	this->buffer_deleter->cycle();
	this->uniform_deleter->cycle();
	this->texture_deleter->cycle();
	this->shader_deleter->cycle();
	//this->framebuffer_set_deleter->cycle();
	//this->single_command_buffer_deleter->cycle();
	this->view_deleter->cycle();
}

VertexBuffer VulkanBackend::createVertexBuffer(void* data, uint64_t data_size, VertexInputDescription& vertex_input_description, MemoryUsage memory_usage)
{
	VulkanVertexBuffer* vertex_buffer = new VulkanVertexBuffer(this->device, data_size, getMemoryUsage(memory_usage), vertex_input_description);

	if (memory_usage == MemoryUsage::GPU_Only)
	{
		VulkanBuffer* staging_buffer = new VulkanBuffer(this->device, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->fillBuffer(data, data_size);

		this->frames[this->frame_index].transfer_buffer->copyBuffer(staging_buffer, 0, (VulkanBuffer*)vertex_buffer, 0, data_size);
	}
	else
	{
		vertex_buffer->fillBuffer(data, data_size);
	}

	return (VertexBuffer)vertex_buffer;
}

void VulkanBackend::destroyVertexBuffer(VertexBuffer vertex_buffer)
{
	this->buffer_deleter->addToQueue((VulkanBuffer*)vertex_buffer);
}

IndexBuffer VulkanBackend::createIndexBuffer(void* data, uint64_t data_size, IndexType type, MemoryUsage memory_usage)
{
	VulkanIndexBuffer* index_buffer = new VulkanIndexBuffer(this->device, data_size, getMemoryUsage(memory_usage), type);

	if (memory_usage == MemoryUsage::GPU_Only)
	{
		VulkanBuffer* staging_buffer = new VulkanBuffer(this->device, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->fillBuffer(data, data_size);

		this->frames[this->frame_index].transfer_buffer->copyBuffer(staging_buffer, 0, (VulkanBuffer*)index_buffer, 0, data_size);
	}
	else
	{
		index_buffer->fillBuffer(data, data_size);
	}

	return (IndexBuffer)index_buffer;
}

void VulkanBackend::destroyIndexBuffer(IndexBuffer index_buffer)
{
	this->buffer_deleter->addToQueue((VulkanBuffer*)index_buffer);
}

UniformBuffer VulkanBackend::createUniformBuffer(uint64_t data_size, MemoryUsage memory_usage)
{
	return (UniformBuffer)new VulkanUniformBuffer(this->device, data_size, getMemoryUsage(memory_usage), this->FRAME_COUNT);
}

void VulkanBackend::destroyUniformBuffer(UniformBuffer uniform_buffer)
{
	this->uniform_deleter->addToQueue((VulkanUniformBuffer*)uniform_buffer);
}

void VulkanBackend::setUniform(UniformBuffer uniform_buffer, void* data, uint64_t data_size)
{
	VulkanUniformBuffer* vulkan_uniform = (VulkanUniformBuffer*)uniform_buffer;
	vulkan_uniform->incrementIndex();
	VulkanBuffer* buffer = vulkan_uniform->getCurrentBuffer();

	if (buffer->isHostVisable() == true)
	{
		buffer->fillBuffer(data, data_size);
	}
	else
	{
		VulkanBuffer* staging_buffer = new VulkanBuffer(this->device, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->fillBuffer(data, data_size);

		this->frames[this->frame_index].transfer_buffer->copyBuffer(staging_buffer, 0, buffer, 0, data_size);
	}
}

Texture VulkanBackend::createTexture(vector2U size, void* data, uint64_t data_size)
{
	VulkanTexture* texture = new VulkanTexture(this->device, {size.x, size.y}, getMemoryUsage(MemoryUsage::GPU_Only));

	VulkanBuffer* staging_buffer = new VulkanBuffer(this->device, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
	staging_buffer->fillBuffer(data, data_size);

	this->frames[this->frame_index].transfer_buffer->fillTexture(staging_buffer, texture);

	return texture;
}

void VulkanBackend::destroyTexture(Texture texture)
{
	this->texture_deleter->addToQueue((VulkanTexture*)texture);
}

Shader VulkanBackend::createShader(string& vert_data, string& frag_data)
{
	return new VulkanShader(this->device->get(), vert_data, frag_data, this->layout_pool);
}

void VulkanBackend::destroyShader(Shader shader)
{
	this->shader_deleter->addToQueue((VulkanShader*)shader);
}

/*Framebuffer VulkanBackend::createFramebuffer(FramebufferLayout& layout, vector2U size)
{
	List<VkFormat> color(layout.getColorCount());
	List<VkClearValue> clear_colors(color.size());
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

	VkRenderPass render_pass = this->render_pass_pool->getRenderPass(layout.getHash(), color, depth);

	VulkanFramebufferSet* frame_buffer = new VulkanFramebufferSet(this->device, { size.x, size.y }, color, depth, render_pass, this->FRAME_COUNT);
	frame_buffer->setClearValues(clear_colors);
	return (Framebuffer)frame_buffer;
}

void VulkanBackend::destroyFramebuffer(Framebuffer frame_buffer)
{
	this->framebuffer_set_deleter->addToQueue((VulkanFramebufferSet*) frame_buffer);
}

void VulkanBackend::resizeFramebuffer(Framebuffer frame_buffer, vector2U size)
{
	((VulkanFramebufferSet*)frame_buffer)->setSize({size.x, size.y});
}

CommandBuffer* VulkanBackend::createCommandBuffer()
{
	return new VulkanCommandBufferSingle(this->device, this->primary_graphics_pool, this->thread_pipeline_pools[0], this->descriptor_pools[0], this->FRAME_COUNT);
}

void VulkanBackend::destroyCommandBuffer(CommandBuffer* command_buffer)
{
	this->single_command_buffer_deleter->addToQueue((VulkanCommandBufferSingle*)command_buffer);
}

void VulkanBackend::beginCommandBuffer(CommandBuffer* command_buffer, Framebuffer target)
{
	if (target != nullptr)
	{
		VulkanFramebufferSet* framebuffer_set = ((VulkanFramebufferSet*)target);
		VulkanFramebuffer* framebuffer = framebuffer_set->getFrameBuffer(this->frame_index);

		VkRect2D rect = {};
		rect.offset = { 0, 0 };
		rect.extent = framebuffer->getSize();
		((VulkanCommandBufferSingle*)command_buffer)->command_buffer.startPrimary(this->frame_index, framebuffer->get(), framebuffer->getRenderPass(), rect, framebuffer_set->getClearValues(), VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
	}
	else
	{
		//Use screen
		VkFramebuffer screen = this->swapchain->getFramebuffer(this->swapchain_image_index);
		VkRenderPass screen_render_pass = this->swapchain->getRenderPass();
		VkRect2D rect = {};
		rect.offset = { 0, 0 };
		rect.extent = this->swapchain->getSwapchainExtent();

		List<VkClearValue> clear_values(1);
		clear_values[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };

		((VulkanCommandBufferSingle*)command_buffer)->command_buffer.startPrimary(this->frame_index, screen, screen_render_pass, rect, clear_values, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
	}
}

void VulkanBackend::endCommandBuffer(CommandBuffer* command_buffer)
{
	((VulkanCommandBufferSingle*)command_buffer)->command_buffer.end();
}

void VulkanBackend::submitCommandBuffer(CommandBuffer* command_buffer)
{
	//((VulkanCommandBufferSingle*)command_buffer)->command_buffer.submit(this->device->getGraphicsQueue(), {}, {}, {}, VK_NULL_HANDLE);
}*/

/*View VulkanBackend::createView(FramebufferLayout& layout, vector2U size)
{
	return View();
}

void VulkanBackend::destroyView(View view)
{
}

void VulkanBackend::resizeView(View view, vector2U size)
{
}

void VulkanBackend::beginView(View view)
{
}

void VulkanBackend::endView(View view)
{
}

void VulkanBackend::submitView(View view, View * sub_views, size_t sub_view_count)
{
}*/

View VulkanBackend::createView(FramebufferLayout& layout, vector2U size)
{
	List<VkFormat> color(layout.getColorCount());
	List<VkClearValue> clear_colors(color.size());
	for (size_t i = 0; i < color.size(); i++)
	{
		color[i] = getImageFormat(layout.getColor(i));
	}
	VkFormat depth = getImageFormat(layout.getDepth());

	VkRenderPass render_pass = this->render_pass_pool->getRenderPass(layout.getHash(), color, depth);

	return (View*) new VulkanViewSingleThread(this->device, this->FRAME_COUNT, this->primary_graphics_pool, this->thread_pipeline_pools[0], this->descriptor_pools[0], {size.x, size.y}, color, depth, render_pass);

}

void VulkanBackend::destroyView(View view)
{
	this->view_deleter->addToQueue((VulkanViewSingleThread*) view);
}

void VulkanBackend::resizeView(View view, vector2U size)
{
	((VulkanViewSingleThread*)view)->setSize({ size.x, size.y });
}

CommandBuffer* VulkanBackend::beginView(View view)
{
	VulkanViewSingleThread* vulkan_view = (VulkanViewSingleThread*)view;
	vulkan_view->start(this->frame_index);
	return vulkan_view->getCommandBuffer(this->frame_index);
}

void VulkanBackend::endView(View view)
{
	VulkanViewSingleThread* vulkan_view = (VulkanViewSingleThread*)view;
	vulkan_view->end();

	//TODO submit
}

void VulkanBackend::waitTillDone()
{
	this->device->waitIdle();
}
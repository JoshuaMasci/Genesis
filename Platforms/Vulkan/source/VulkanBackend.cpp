#include "VulkanBackend.hpp"

#include "Genesis/Debug/Assert.hpp"
#include "Genesis/Debug/Profiler.hpp"

#include "VulkanDescriptorSet.hpp"

#include "VulkanPhysicalDevicePicker.hpp"

using namespace Genesis;

inline VkBufferUsageFlags getBufferUseage(BufferUsage usage, MemoryType memory_usage)
{
	VkBufferUsageFlags buffer_usage = 0;

	switch (usage)
	{
	case BufferUsage::Vertex_Buffer:
		buffer_usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		break;
	case BufferUsage::Index_Buffer:
		buffer_usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		break;
	case BufferUsage::Uniform_Buffer:
		buffer_usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		break;
	case BufferUsage::Storage_Buffer:
		buffer_usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		break;
	}

	if (memory_usage == MemoryType::GPU_Only)
	{
		buffer_usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}

	return buffer_usage;
}

inline VmaMemoryUsage getMemoryUsage(MemoryType memory_usage)
{
	switch (memory_usage)
	{
	case MemoryType::GPU_Only:
		return VMA_MEMORY_USAGE_GPU_ONLY;
	case MemoryType::CPU_Visable:
		return VMA_MEMORY_USAGE_CPU_TO_GPU;
	}
	return VMA_MEMORY_USAGE_UNKNOWN;
}

inline VkFormat getImageFormat(ImageFormat format)
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

	uint32_t count;
	vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr); //get number of extensions
	std::vector<VkExtensionProperties> list_extensions(count);
	vkEnumerateInstanceExtensionProperties(nullptr, &count, list_extensions.data());

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
	this->transfer_pool = new VulkanCommandPool(this->device->get(), this->device->getGraphicsFamilyIndex(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	//Descriptor Pools
	this->descriptor_pool = new VulkanDescriptorPool2(this->device->get(), 8000,
		{
			{VK_DESCRIPTOR_TYPE_SAMPLER, 8000},
			{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 8000},
			{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 8000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 8000},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 8000}
		});

	this->descriptor_pools.resize(this->THREAD_COUNT);
	for (size_t i = 0; i < this->descriptor_pools.size(); i++)
	{
		this->descriptor_pools[i] =
			new VulkanDescriptorPool(this->device->get(), this->FRAME_COUNT, 8000,
				{
					{VK_DESCRIPTOR_TYPE_SAMPLER, 8000},
					{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 8000},
					{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 8000},
					{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 8000},
					{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 8000}
				});
	}

	//Layout pool
	this->layout_pool = new VulkanLayoutPool(this->device->get());

	//Pipeline pools
	this->pipeline_pool = new VulkanPipelinePool(this->device->get());
	this->thread_pipeline_pools.resize(this->THREAD_COUNT);
	for (size_t i = 0; i < this->thread_pipeline_pools.size(); i++)
	{
		this->thread_pipeline_pools[i] = new VulkanThreadPipelinePool(this->device->get(), this->pipeline_pool);
	}

	//RenderPass pool
	this->render_pass_pool = new VulkanRenderPassPool(this->device->get());

	//Sampler pool
	this->sampler_pool = new VulkanSamplerPool(this->device->get());
	this->vertex_input_pool = new VulkanVertexInputPool();

	this->transfer_buffers.resize(this->FRAME_COUNT);
	for (size_t i = 0; i < this->transfer_buffers.size(); i++)
	{
		this->transfer_buffers[i] = new VulkanTransferBuffer(this->device, this->transfer_pool);
	}

	this->frames.resize(this->FRAME_COUNT);
	for (size_t i = 0; i < this->frames.size(); i++)
	{
		this->frames[i].image_ready_semaphore = this->device->createSemaphore();
		this->frames[i].command_buffer = new VulkanCommandBuffer(this->device, this->primary_graphics_pool, this->thread_pipeline_pools[0], this->descriptor_pools[0], this->transfer_buffers[i], (uint32_t)i);
		this->frames[i].command_buffer_done_semaphore = this->device->createSemaphore();
		this->frames[i].frame_done_fence = this->device->createFence();
	}

	const uint8_t delay_cycles = (uint8_t)this->FRAME_COUNT;
	this->buffer_deleter = new DelayedResourceDeleter<VulkanBuffer>(delay_cycles);
	this->dynamic_deleter = new DelayedResourceDeleter<VulkanDynamicBuffer>(delay_cycles);
	this->texture_deleter = new DelayedResourceDeleter<VulkanTexture>(delay_cycles);
	this->shader_deleter = new DelayedResourceDeleter<VulkanShader>(delay_cycles);
	this->frame_deleter = new DelayedResourceDeleter<VulkanFramebufferSet>(delay_cycles);
	this->st_command_buffer_deleter = new DelayedResourceDeleter<VulkanCommandBufferSet>(delay_cycles);
	this->mt_command_buffer_deleter = new DelayedResourceDeleter<VulkanCommandBufferMultithreadSet>(delay_cycles);
	this->descriptor_set_deleter = new DelayedResourceDeleter<VulkanDescriptorSet>(delay_cycles);
}

VulkanBackend::~VulkanBackend()
{
	delete this->buffer_deleter;
	delete this->dynamic_deleter;
	delete this->texture_deleter;
	delete this->shader_deleter;
	delete this->frame_deleter;
	delete this->mt_command_buffer_deleter;
	delete this->descriptor_set_deleter;

	for (size_t i = 0; i < this->frames.size(); i++)
	{
		this->device->destroySemaphore(this->frames[i].image_ready_semaphore);
		delete this->frames[i].command_buffer;
		this->device->destroySemaphore(this->frames[i].command_buffer_done_semaphore);
		this->device->destroyFence(this->frames[i].frame_done_fence);
	}

	for (size_t i = 0; i < this->transfer_buffers.size(); i++)
	{
		delete this->transfer_buffers[i];
	}

	delete this->render_pass_pool;

	delete this->vertex_input_pool;
	delete this->sampler_pool;

	for (size_t i = 0; i < this->thread_pipeline_pools.size(); i++)
	{
		delete this->thread_pipeline_pools[i];
	}
	delete this->pipeline_pool;

	delete this->layout_pool;

	delete this->descriptor_pool;
	for (size_t i = 0; i < this->descriptor_pools.size(); i++)
	{
		delete this->descriptor_pools[i];
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
	VkExtent2D size = this->swapchain->getSwapchainExtent();
	return vector2U(size.width, size.height);
}

CommandBufferInterface* VulkanBackend::beginFrame()
{
	GENESIS_PROFILE_FUNCTION("VulkanBackend_beginFrame");

	if (this->swapchain->invalid())
	{
		vector2U surface_size = this->window->getWindowSize();
		this->swapchain->recreateSwapchain({ surface_size.x, surface_size.y });
	}

	uint32_t index = this->swapchain->getNextImage(this->frames[this->frame_index].image_ready_semaphore);

	if (index == std::numeric_limits<uint32_t>::max())
	{
		return nullptr;
	}

	this->swapchain_image_index = index;

	vkWaitForFences(this->device->get(), 1, &this->frames[this->frame_index].frame_done_fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(this->device->get(), 1, &this->frames[this->frame_index].frame_done_fence);

	//Start TransferPool for this frame
	this->transfer_buffers[this->frame_index]->reset();

	for (size_t i = 0; i < this->descriptor_pools.size(); i++)
	{
		this->descriptor_pools[i]->resetFrame(this->frame_index);
	}

	List<VkClearValue> clear_values(1);
	clear_values[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	VkRect2D render_area = {};
	render_area.offset = { 0, 0 };
	render_area.extent = this->swapchain->getSwapchainExtent();
	this->frames[this->frame_index].command_buffer->startPrimary(this->swapchain->getFramebuffer(this->swapchain_image_index), this->swapchain->getRenderPass(), render_area, clear_values, VK_SUBPASS_CONTENTS_INLINE);

	return this->frames[this->frame_index].command_buffer;
}

void VulkanBackend::endFrame()
{
	GENESIS_PROFILE_FUNCTION("VulkanBackend_endFrame");

	//End main command buffer
	this->frames[this->frame_index].command_buffer->endPrimary();

	//Add main command buffer to list
	this->graphics_command_buffers.push_back(this->frames[this->frame_index].command_buffer->getCommandBuffer());

	//Excute pending transfers
	bool has_transfers = this->transfer_buffers[this->frame_index]->SubmitTransfers(this->device->getTransferQueue());

	List<VkSemaphore> wait_semaphores(1);
	List<VkPipelineStageFlags> wait_stages(1);

	wait_semaphores[0] = this->frames[this->frame_index].image_ready_semaphore;
	wait_stages[0] = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

	if (has_transfers)
	{
		wait_semaphores.resize(2);
		wait_stages.resize(2);

		wait_semaphores[1] = this->transfer_buffers[this->frame_index]->getTransferDoneSemaphore();
		wait_stages[1] = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
	}

	List<VkSemaphore> signal_semaphores(1);
	signal_semaphores[0] = this->frames[this->frame_index].command_buffer_done_semaphore;

	//Create submition info
	VkSubmitInfo graphics_submit_info = {};
	graphics_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	graphics_submit_info.waitSemaphoreCount = (uint32_t)wait_semaphores.size();
	graphics_submit_info.pWaitSemaphores = wait_semaphores.data();
	graphics_submit_info.pWaitDstStageMask = wait_stages.data();

	graphics_submit_info.signalSemaphoreCount = (uint32_t)signal_semaphores.size();
	graphics_submit_info.pSignalSemaphores = signal_semaphores.data();

	graphics_submit_info.commandBufferCount = (uint32_t)this->graphics_command_buffers.size();
	graphics_submit_info.pCommandBuffers = this->graphics_command_buffers.data();

	//Submit command buffers
	GENESIS_ENGINE_ASSERT_ERROR((vkQueueSubmit(this->device->getGraphicsQueue(), 1, &graphics_submit_info, this->frames[this->frame_index].frame_done_fence) == VK_SUCCESS), "failed to submit command buffer");

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

	//Clear the command buffers
	this->graphics_command_buffers.clear();

	this->frame_index = (this->frame_index + 1) % this->FRAME_COUNT;

	this->buffer_deleter->cycle();
	this->dynamic_deleter->cycle();
	this->texture_deleter->cycle();
	this->shader_deleter->cycle();
	this->frame_deleter->cycle();
	this->st_command_buffer_deleter->cycle();
	this->mt_command_buffer_deleter->cycle();
	this->descriptor_set_deleter->cycle();

	this->pipeline_pool->update();
}

Sampler VulkanBackend::createSampler(const SamplerCreateInfo& create_info)
{
	return (Sampler)this->sampler_pool->getSampler(create_info);
}

VertexInputDescription VulkanBackend::createVertexInputDescription(vector<VertexElementType> input_elements)
{
	return (VertexInputDescription)this->vertex_input_pool->getVertexInputDescription(input_elements);
}

VertexInputDescription VulkanBackend::createVertexInputDescription(const VertexInputDescriptionCreateInfo& create_info)
{
	return (VertexInputDescription)this->vertex_input_pool->getVertexInputDescription(create_info);
}

DescriptorSetLayout VulkanBackend::createDescriptorSetLayout(const DescriptorSetLayoutCreateInfo& create_info)
{
	return (DescriptorSetLayout)this->layout_pool->getDescriptorLayout(create_info);
}

DescriptorSet VulkanBackend::createDescriptorSet(const DescriptorSetCreateInfo& create_info)
{
	return (DescriptorSet) new VulkanDescriptorSet(this->device->get(), this->descriptor_pool, create_info);
}

void VulkanBackend::destroyDescriptorSet(DescriptorSet descriptor_set)
{
	this->descriptor_set_deleter->addToQueue((VulkanDescriptorSet*)descriptor_set);
}

PipelineLayout VulkanBackend::createPipelineLayout(const PipelineLayoutCreateInfo& create_info)
{
	return (PipelineLayout)this->layout_pool->getPipelineLayout(create_info);
}

ShaderModule VulkanBackend::createShaderModule(ShaderModuleCreateInfo& create_info)
{
	return (ShaderModule)new VulkanShaderModule(this->device->get(), create_info);
}

void VulkanBackend::destroyShaderModule(ShaderModule shader_module)
{
	//TODO
}

StaticBuffer VulkanBackend::createStaticBuffer(void* data, uint64_t data_size, BufferUsage buffer_usage, MemoryType memory_usage)
{
	VulkanBuffer* buffer = new VulkanBuffer(this->device, data_size, getBufferUseage(buffer_usage, memory_usage), getMemoryUsage(memory_usage));

	if(buffer->isHostVisable())
	{
		buffer->fillBuffer(data, data_size);
	}
	else
	{
		VulkanBuffer* staging_buffer = new VulkanBuffer(this->device, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->fillBuffer(data, data_size);

		this->transfer_buffers[this->frame_index]->copyBuffer(staging_buffer, 0, (VulkanBuffer*)buffer, 0, data_size);
		this->buffer_deleter->addToQueue(staging_buffer);
	}

	return (StaticBuffer) buffer;
}

void VulkanBackend::destroyStaticBuffer(StaticBuffer buffer)
{
	this->buffer_deleter->addToQueue((VulkanBuffer*)buffer);
}

DynamicBuffer VulkanBackend::createDynamicBuffer(uint64_t data_size, BufferUsage buffer_usage, MemoryType memory_usage)
{
	return (DynamicBuffer)new VulkanDynamicBuffer(this->device, data_size, getBufferUseage(buffer_usage, memory_usage), getMemoryUsage(memory_usage), this->FRAME_COUNT);
}

void VulkanBackend::destroyDynamicBuffer(DynamicBuffer buffer)
{
	this->dynamic_deleter->addToQueue((VulkanDynamicBuffer*)buffer);
}

void VulkanBackend::writeDynamicBuffer(DynamicBuffer buffer, void * data, uint64_t data_size)
{
	VulkanDynamicBuffer* dynamic_buffer = (VulkanDynamicBuffer*)buffer;
	dynamic_buffer->incrementIndex();
	VulkanBuffer* current_buffer = dynamic_buffer->getCurrentBuffer();

	if (current_buffer->isHostVisable())
	{
		current_buffer->fillBuffer(data, data_size);
	}
	else
	{
		VulkanBuffer* staging_buffer = new VulkanBuffer(this->device, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->fillBuffer(data, data_size);

		this->transfer_buffers[this->frame_index]->copyBuffer(staging_buffer, 0, current_buffer, 0, data_size);
		this->buffer_deleter->addToQueue(staging_buffer);
	}
}

Texture VulkanBackend::createTexture(vector2U size, void* data, uint64_t data_size)
{
	VulkanTexture* texture = new VulkanTexture(this->device, {size.x, size.y}, getMemoryUsage(MemoryType::GPU_Only));

	VulkanBuffer* staging_buffer = new VulkanBuffer(this->device, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
	staging_buffer->fillBuffer(data, data_size);

	this->transfer_buffers[this->frame_index]->fillTexture(staging_buffer, texture);
	this->buffer_deleter->addToQueue(staging_buffer);

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

Framebuffer VulkanBackend::createFramebuffer(FramebufferLayout& layout, vector2U size)
{
	List<VkFormat> color(layout.getColorCount());
	List<VkClearValue> clear_colors(color.size());
	for (size_t i = 0; i < color.size(); i++)
	{
		color[i] = getImageFormat(layout.getColor(i));
	}
	VkFormat depth = getImageFormat(layout.getDepth());

	VkRenderPass render_pass = this->render_pass_pool->getRenderPass(layout.getHash(), color, depth);

	return new VulkanFramebufferSet(this->device, { size.x, size.y }, color, depth, render_pass, this->FRAME_COUNT);
}

void VulkanBackend::destroyFramebuffer(Framebuffer framebuffer)
{
	this->frame_deleter->addToQueue((VulkanFramebufferSet*)framebuffer);
}

void VulkanBackend::resizeFramebuffer(Framebuffer framebuffer, vector2U size)
{
	((VulkanFramebufferSet*)framebuffer)->size = {size.x, size.y};
}

STCommandBuffer VulkanBackend::createSTCommandBuffer()
{
	return (STCommandBuffer)new VulkanCommandBufferSet(this->device, this->primary_graphics_pool, this->thread_pipeline_pools[0], this->descriptor_pools[0], this->transfer_buffers[0], this->FRAME_COUNT);
}

void VulkanBackend::destroySTCommandBuffer(STCommandBuffer st_command_buffer)
{
	this->st_command_buffer_deleter->addToQueue((VulkanCommandBufferSet*)st_command_buffer);
}

CommandBufferInterface* VulkanBackend::beginSTCommandBuffer(STCommandBuffer st_command_buffer, Framebuffer framebuffer_target)
{
	VulkanCommandBufferSet* command_buffer = (VulkanCommandBufferSet*)st_command_buffer;
	((VulkanFramebufferSet*)framebuffer_target)->rebuildFramebuffer(this->frame_index);
	VulkanFramebuffer* framebuffer = ((VulkanFramebufferSet*)framebuffer_target)->framebuffers[this->frame_index];

	command_buffer->command_buffers[this->frame_index]->startPrimary(framebuffer->get(), framebuffer->getRenderPass(), { {0, 0}, framebuffer->getSize() }, framebuffer->getClearValues(), VK_SUBPASS_CONTENTS_INLINE);
	return command_buffer->command_buffers[this->frame_index];
}

void VulkanBackend::endSTCommandBuffer(STCommandBuffer st_command_buffer)
{
	VulkanCommandBufferSet* command_buffer = (VulkanCommandBufferSet*)st_command_buffer;
	command_buffer->command_buffers[this->frame_index]->endPrimary();
	this->graphics_command_buffers.push_back(command_buffer->command_buffers[this->frame_index]->getCommandBuffer());
}

MTCommandBuffer VulkanBackend::createMTCommandBuffer()
{
	return new VulkanCommandBufferMultithreadSet(this->device, this->FRAME_COUNT, this->THREAD_COUNT, this->primary_graphics_pool, this->secondary_graphics_pools, this->thread_pipeline_pools, this->descriptor_pools, this->transfer_buffers);
}

void VulkanBackend::destroyMTCommandBuffer(MTCommandBuffer mt_command_buffer)
{
	this->mt_command_buffer_deleter->addToQueue((VulkanCommandBufferMultithreadSet*)mt_command_buffer);
}

List<CommandBufferInterface*>* VulkanBackend::beginMTCommandBuffer(MTCommandBuffer mt_command_buffer, Framebuffer framebuffer_target)
{
	VulkanCommandBufferMultithreadSet* command_buffer = (VulkanCommandBufferMultithreadSet*)mt_command_buffer;

	((VulkanFramebufferSet*)framebuffer_target)->rebuildFramebuffer(this->frame_index);
	VulkanFramebuffer* vulkan_framebuffer = ((VulkanFramebufferSet*)framebuffer_target)->framebuffers[this->frame_index];

	command_buffer->command_buffers[this->frame_index]->start(vulkan_framebuffer);
	return (List<CommandBufferInterface*>*)command_buffer->command_buffers[this->frame_index]->getSecondaryCommandBuffers();
}

void VulkanBackend::endMTCommandBuffer(MTCommandBuffer mt_command_buffer)
{
	VulkanCommandBufferMultithreadSet* command_buffer = (VulkanCommandBufferMultithreadSet*)mt_command_buffer;
	command_buffer->command_buffers[this->frame_index]->end();
	this->graphics_command_buffers.push_back(command_buffer->command_buffers[this->frame_index]->getCommandBuffer());
}

void VulkanBackend::waitTillDone()
{
	this->device->waitIdle();
}
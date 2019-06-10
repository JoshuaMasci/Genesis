#include "VulkanBackend.hpp"

#define VMA_IMPLEMENTATION

#include "Genesis/Rendering/Vulkan/VulkanInstance.hpp"
#include "Genesis/Rendering/Vulkan/VulkanBuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanRenderPipline.hpp"

#include "Genesis/Rendering/Vulkan/VulkanImageUtils.hpp"

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
}

VulkanBackend::~VulkanBackend()
{
	delete this->vulkan;
}

bool VulkanBackend::beginFrame()
{
	VulkanFrame* frame = &this->vulkan->frames_in_flight[this->frame_index];
	bool image_acquired = this->vulkan->AcquireSwapchainImage(this->swapchain_image_index, frame->image_available_semaphore);

	if (image_acquired == false)
	{
		return false;
	}

	vkWaitForFences(this->vulkan->device->getDevice(), 1, &frame->command_buffer_done_fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(this->vulkan->device->getDevice(), 1, &frame->command_buffer_done_fence);

	VkRenderPassBeginInfo renderPassInfo = {};
	{
		VkExtent2D swapchain_extent = this->vulkan->swapchain->getSwapchainExtent();
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = this->vulkan->pipeline_manager->getScreenRenderPass();
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
	VulkanFrame* frame = &this->vulkan->frames_in_flight[this->frame_index];

	frame->command_buffer->endCommandBuffer();

	if (this->vulkan->swapchain == nullptr)
	{
		return;
	}

	Array<VkSemaphore> wait_semaphores(1);
	Array<VkPipelineStageFlags> wait_states(1);
	wait_semaphores[0] = frame->image_available_semaphore;
	wait_states[0] = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

	Array<VkSemaphore> signal_semaphores(1);
	signal_semaphores[0] = frame->command_buffer_done_semaphore;

	frame->command_buffer->submitCommandBuffer(this->vulkan->device->getGraphicsQueue(), wait_semaphores, wait_states, signal_semaphores, frame->command_buffer_done_fence);

	//Present the image to the screen
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &frame->command_buffer_done_semaphore;

	VkSwapchainKHR swapChains[] = { this->vulkan->swapchain->getSwapchain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &this->swapchain_image_index;

	VkResult result = vkQueuePresentKHR(this->vulkan->device->getPresentQueue(), &presentInfo);

	this->frame_index = (this->frame_index + 1) % this->vulkan->frames_in_flight.size();
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

	VulkanFrame* frame = &this->vulkan->frames_in_flight[this->frame_index];

	VkCommandBuffer buffer = frame->command_buffer->getSecondaryCommandBuffer(thread);

	vkCmdPushConstants(buffer, this->vulkan->pipeline_manager->colored_mesh_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(matrix4F), &mvp);
	vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->vulkan->pipeline_manager->colored_mesh_screen_pipeline->getPipeline());

	VkBuffer vertexBuffers[] = { (VkBuffer)vertex_buffer->getHandle() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(buffer, (VkBuffer)index_buffer->getHandle(), 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(buffer, index_count, 1, 0, 0, 0);
}

void VulkanBackend::waitTillDone()
{
	vkDeviceWaitIdle(this->vulkan->device->getDevice());
}

TextureIndex VulkanBackend::createTexture(vector2U size)
{
	TextureIndex texture_index = this->vulkan->next_index;
	this->vulkan->next_index++;

	VulkanTexture* texture = &this->vulkan->textures[texture_index];

	VkImageCreateInfo image_info = {};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = size.x;
	image_info.extent.height = size.y;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;
	image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	image_info.format = VK_FORMAT_R8G8B8A8_UNORM;
	image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_info.flags = 0;

	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage = getMemoryUsage(MemoryUsage::GPU_Only);

	vmaCreateImage(this->vulkan->allocator, &image_info, &alloc_info, &texture->image, &texture->image_memory, &texture->image_memory_info);
	texture->size = { size.x, size.y };

	return texture_index;
}

void VulkanBackend::fillTexture(TextureIndex texture_index, void* data, uint64_t data_size)
{
	if (texture_index == NULL_INDEX)
	{
		printf("Error: null texture index");
		return;
	}

	if (this->vulkan->textures.find(texture_index) == this->vulkan->textures.end())
	{
		printf("Error: invalid texture index");
		return;
	}

	VulkanTexture* texture = &this->vulkan->textures[texture_index];

	if (data_size != texture->size.width * texture->size.height * 4)
	{
		printf("Error: image not correct size for texture");
		return;
	}

	//Staging Buffer
	VkBuffer staging_buffer;
	VmaAllocation staging_buffer_memory;
	VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = data_size;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	vmaCreateBuffer(this->vulkan->allocator, &bufferInfo, &allocInfo, &staging_buffer, &staging_buffer_memory, nullptr);

	void* mappedData;
	vmaMapMemory(this->vulkan->allocator, staging_buffer_memory, &mappedData);
	memcpy(mappedData, data, data_size);
	vmaUnmapMemory(this->vulkan->allocator, staging_buffer_memory);

	transitionImageLayout(this->vulkan->command_pool, texture->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(this->vulkan->command_pool, staging_buffer, texture->image, texture->size);
	transitionImageLayout(this->vulkan->command_pool, texture->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vmaDestroyBuffer(this->vulkan->allocator, staging_buffer, staging_buffer_memory);
}

void VulkanBackend::destroyTexture(TextureIndex texture_index)
{
	if (this->vulkan->textures.find(texture_index) != this->vulkan->textures.end())
	{
		VulkanTexture* texture = &this->vulkan->textures[texture_index];
		vmaDestroyImage(this->vulkan->allocator, texture->image, texture->image_memory);

		this->vulkan->textures.erase(texture_index);
	}
}
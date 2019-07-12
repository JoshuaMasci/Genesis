#include "VulkanBackend.hpp"

#include "Genesis/Rendering/Vulkan/VulkanInstance.hpp"
#include "Genesis/Rendering/Vulkan/VulkanRenderPipline.hpp"
#include "Genesis/Rendering/Vulkan/VulkanImageUtils.hpp"

#include "Genesis/Core/VectorTypes.hpp"

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

	vkWaitForFences(this->vulkan->device->get(), 1, &frame->command_buffer_done_fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(this->vulkan->device->get(), 1, &frame->command_buffer_done_fence);

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
	if (this->vulkan->swapchain == nullptr)
	{
		return;
	}

	VulkanFrame* frame = &this->vulkan->frames_in_flight[this->frame_index];

	frame->command_buffer->endCommandBuffer();
}

void VulkanBackend::submitFrame()
{
	if (this->vulkan->swapchain == nullptr)
	{
		return;
	}

	VulkanFrame* frame = &this->vulkan->frames_in_flight[this->frame_index];

	Array<VkSemaphore> wait_semaphores(1);
	wait_semaphores[0] = frame->image_available_semaphore;

	Array<VkPipelineStageFlags> wait_states(1);
	wait_states[0] = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

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
}

BufferIndex VulkanBackend::createBuffer(uint64_t size_bytes, BufferType type, MemoryUsage memory_usage)
{
	BufferIndex buffer_index = this->vulkan->next_index_buffer;
	this->vulkan->next_index_buffer++;

	VulkanBuffer* buffer = &this->vulkan->buffers[buffer_index];

	VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	buffer_info.size = size_bytes;
	buffer_info.usage = getBufferUsage(type);

	this->vulkan->allocator->createBuffer(&buffer_info, getMemoryUsage(memory_usage), &buffer->buffer, &buffer->buffer_memory, &buffer->buffer_memory_info);

	return buffer_index;
}

void VulkanBackend::fillBuffer(BufferIndex buffer_index, void* data, uint64_t data_size)
{
	if (buffer_index == NULL_INDEX)
	{
		printf("Error: null buffer index\n");
		return;
	}

	if (this->vulkan->buffers.find(buffer_index) == this->vulkan->buffers.end())
	{
		printf("Error: invalid buffer index\n");
		return;
	}

	VulkanBuffer* buffer = &this->vulkan->buffers[buffer_index];

	VkDeviceSize buffer_size = buffer->buffer_memory_info.size;

	if (this->vulkan->allocator->isMemoryHostVisible(buffer->buffer_memory_info))
	{
		void* mapped_data;
		this->vulkan->allocator->mapMemory(buffer->buffer_memory, &mapped_data);
		memcpy(mapped_data, data, data_size);
		this->vulkan->allocator->unmapMemory(buffer->buffer_memory);
	}
	else
	{
		//Staging Buffer
		VkBuffer staging_buffer;
		VmaAllocation staging_buffer_memory;
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = data_size;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		this->vulkan->allocator->createBuffer(&bufferInfo, VMA_MEMORY_USAGE_CPU_ONLY, &staging_buffer, &staging_buffer_memory, nullptr);

		void* mapped_data;
		this->vulkan->allocator->mapMemory(staging_buffer_memory, &mapped_data);
		memcpy(mapped_data, data, data_size);
		this->vulkan->allocator->unmapMemory(staging_buffer_memory);

		VkCommandBuffer command_buffer = this->vulkan->primary_command_pool->getCommandBuffer();
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(command_buffer, &begin_info);

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = data_size;
		vkCmdCopyBuffer(command_buffer, staging_buffer, buffer->buffer, 1, &copyRegion);

		vkEndCommandBuffer(command_buffer);
		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		vkQueueSubmit(this->vulkan->device->getGraphicsQueue(), 1, &submit_info, VK_NULL_HANDLE);
		vkQueueWaitIdle(this->vulkan->device->getGraphicsQueue());
		this->vulkan->primary_command_pool->freeCommandBuffer(command_buffer);

		this->vulkan->allocator->destroyBuffer(staging_buffer, staging_buffer_memory);
	}
}

void VulkanBackend::destroyBuffer(BufferIndex buffer_index)
{
	if (this->vulkan->buffers.find(buffer_index) == this->vulkan->buffers.end())
	{
		printf("Error: buffer doesn't exsist\n");
		return;
	}

	VulkanBuffer* buffer = &this->vulkan->buffers[buffer_index];
	this->vulkan->allocator->destroyBuffer(buffer->buffer, buffer->buffer_memory);

	this->vulkan->buffers.erase(buffer_index);
}

TextureIndex VulkanBackend::createTexture(vector2U size)
{
	TextureIndex texture_index = this->vulkan->next_index_texture;
	this->vulkan->next_index_texture++;

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

	this->vulkan->allocator->createImage(&image_info, getMemoryUsage(MemoryUsage::GPU_Only), &texture->image, &texture->image_memory, &texture->image_memory_info);
	texture->size = { size.x, size.y };

	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = texture->image;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = image_info.format;
	view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;
	if (vkCreateImageView(this->vulkan->device->get(), &view_info, nullptr, &texture->image_view) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture image view!");
	}

	{
		texture->image_descriptor_set = this->vulkan->descriptor_pool->getDescriptorSet();
	
		VkDescriptorImageInfo descriptor_image_info = {};
		descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptor_image_info.imageView = texture->image_view;
		descriptor_image_info.sampler = this->vulkan->linear_sampler;

		VkWriteDescriptorSet descriptor_write = {};
		descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_write.dstSet = texture->image_descriptor_set;
		descriptor_write.dstBinding = 0;
		descriptor_write.dstArrayElement = 0;
		descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_write.descriptorCount = 1;
		descriptor_write.pImageInfo = &descriptor_image_info;

		vkUpdateDescriptorSets(this->vulkan->device->get(), 1, &descriptor_write, 0, nullptr);
	}

	return texture_index;
}

void VulkanBackend::fillTexture(TextureIndex texture_index, void* data, uint64_t data_size)
{
	if (texture_index == NULL_INDEX)
	{
		printf("Error: null texture index\n");
		return;
	}

	if (this->vulkan->textures.find(texture_index) == this->vulkan->textures.end())
	{
		printf("Error: invalid texture index\n");
		return;
	}

	VulkanTexture* texture = &this->vulkan->textures[texture_index];

	const uint32_t bytes_per_pixel = 4;
	if (data_size != texture->size.width * texture->size.height * bytes_per_pixel)
	{
		printf("Error: image not correct size for texture\n");
		return;
	}

	//Staging Buffer
	VkBuffer staging_buffer;
	VmaAllocation staging_buffer_memory;
	VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = data_size;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	this->vulkan->allocator->createBuffer(&bufferInfo, VMA_MEMORY_USAGE_CPU_ONLY, &staging_buffer, &staging_buffer_memory, nullptr);

	void* mapped_data;
	this->vulkan->allocator->mapMemory(staging_buffer_memory, &mapped_data);
	memcpy(mapped_data, data, data_size);
	this->vulkan->allocator->unmapMemory(staging_buffer_memory);

	transitionImageLayout(this->vulkan->primary_command_pool, this->vulkan->device->getGraphicsQueue(), texture->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(this->vulkan->primary_command_pool, this->vulkan->device->getGraphicsQueue(), staging_buffer, texture->image, texture->size);
	transitionImageLayout(this->vulkan->primary_command_pool, this->vulkan->device->getGraphicsQueue(), texture->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	this->vulkan->allocator->destroyBuffer(staging_buffer, staging_buffer_memory);
}

void VulkanBackend::destroyTexture(TextureIndex texture_index)
{
	if (this->vulkan->textures.find(texture_index) != this->vulkan->textures.end())
	{
		VulkanTexture* texture = &this->vulkan->textures[texture_index];
		vkDestroyImageView(this->vulkan->device->get(), texture->image_view, nullptr);
		this->vulkan->allocator->destroyImage(texture->image, texture->image_memory);

		this->vulkan->descriptor_pool->freeDescriptorSet(texture->image_descriptor_set);

		this->vulkan->textures.erase(texture_index);
	}
}

ViewIndex VulkanBackend::createView(ViewType type, vector2U size)
{
	

	return ViewIndex();
}

void VulkanBackend::destroyView(ViewIndex index)
{
}

void VulkanBackend::drawMeshScreen(uint32_t thread, BufferIndex vertices_index, BufferIndex indices_index, TextureIndex texture_index, uint32_t indices_count, matrix4F mvp)
{
	if (this->vulkan->swapchain == nullptr)
	{
		return;
	}

	if (this->vulkan->buffers.find(vertices_index) == this->vulkan->buffers.end())
	{
		printf("Error: invalid vertex buffer\n");
		return;
	}
	VulkanBuffer* vertices_buffer = &this->vulkan->buffers[vertices_index];

	if (this->vulkan->buffers.find(indices_index) == this->vulkan->buffers.end())
	{
		printf("Error: invalid index buffer\n");
		return;
	}
	VulkanBuffer* indices_buffer = &this->vulkan->buffers[indices_index];

	if (this->vulkan->textures.find(texture_index) == this->vulkan->textures.end())
	{
		printf("Error: invalid texture\n");
		return;
	}
	VulkanTexture* texture = &this->vulkan->textures[texture_index];

	VulkanFrame* frame = &this->vulkan->frames_in_flight[this->frame_index];

	VkCommandBuffer buffer = frame->command_buffer->getSecondaryCommandBuffer(thread);

	vkCmdPushConstants(buffer, this->vulkan->pipeline_manager->textured_mesh_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(matrix4F), &mvp);
	vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->vulkan->pipeline_manager->textured_mesh_screen_pipeline->getPipeline());
	
	vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->vulkan->pipeline_manager->textured_mesh_layout, 0, 1, &texture->image_descriptor_set, 0, nullptr);

	VkBuffer vertexBuffers[] = { vertices_buffer->buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(buffer, indices_buffer->buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(buffer, indices_count, 1, 0, 0, 0);
}

matrix4F VulkanBackend::getPerspectiveMatrix(Camera* camera, float aspect_ratio)
{
	float fov = 1.0f / tan(glm::radians(camera->frame_of_view) / 2.0f);
	matrix4F proj = glm::infinitePerspective(fov, aspect_ratio, camera->z_near);
	proj[1][1] *= -1; //Need to apply this because vulkan flips the y-axis and that's not what I need

	return proj;
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

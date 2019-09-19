#include "VulkanBackend.hpp"

#include "Genesis/Rendering/Vulkan/VulkanInstance.hpp"
#include "Genesis/Rendering/Vulkan/VulkanPipline.hpp"
#include "Genesis/Rendering/Vulkan/VulkanView.hpp"

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
	case ImageFormat::RGBA_8_UNorm:
		return VK_FORMAT_R8G8B8A8_UNORM;
		break;
	case ImageFormat::R_16_Float:
		return VK_FORMAT_R16_SFLOAT;
		break;
	case ImageFormat::RG_16_Float:
		return VK_FORMAT_R16G16_SFLOAT;
		break;
	case ImageFormat::RGB_16_Float:
		return VK_FORMAT_R16G16B16_SFLOAT;
		break;
	case ImageFormat::RGBA_16_Float:
		return VK_FORMAT_R16G16B16A16_SFLOAT;
		break;
	case ImageFormat::R_32_Float:
		return VK_FORMAT_R32_SFLOAT;
		break;
	case ImageFormat::RG_32_Float:
		return VK_FORMAT_R32G32_SFLOAT;
		break;
	case ImageFormat::RGB_32_Float:
		return VK_FORMAT_R32G32B32_SFLOAT;
		break;
	case ImageFormat::RGBA_32_Float:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
		break;
	}
	return VK_FORMAT_UNDEFINED;
}

VulkanBackend::VulkanBackend(Window* window, uint32_t number_of_threads)
{
	this->vulkan = new VulkanInstance(window, number_of_threads, 3);
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

	this->vulkan->descriptor_pool->resetFrame(this->frame_index);
	this->vulkan->uniform_pool->resetFrame(this->frame_index);

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
	//frame->command_buffer->beginCommandBuffer(renderPassInfo);
	frame->command_buffer_temp->beginCommandBuffer(renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	return true;
}

void VulkanBackend::endFrame()
{
	if (this->vulkan->swapchain == nullptr)
	{
		return;
	}

	VulkanFrame* frame = &this->vulkan->frames_in_flight[this->frame_index];

	//frame->command_buffer->endCommandBuffer();
	frame->command_buffer_temp->endCommandBuffer();
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

	frame->command_buffer_temp->submitCommandBuffer(this->vulkan->device->getGraphicsQueue(), wait_semaphores, wait_states, signal_semaphores, frame->command_buffer_done_fence);

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
}

VertexBufferHandle VulkanBackend::createVertexBuffer(void* data, uint64_t data_size, VertexInputDescription& vertex_input_description, MemoryUsage memory_usage)
{
	return (VertexBufferHandle) new VulkanVertexBuffer(this->vulkan->allocator, this->vulkan->graphics_command_pool_set->getPrimaryCommandPool(), this->vulkan->device->getGraphicsQueue(), data, data_size, getMemoryUsage(memory_usage), vertex_input_description);
}

void VulkanBackend::destroyVertexBuffer(VertexBufferHandle vertex_buffer_index)
{
	this->vulkan->buffer_deleter->addToQueue((VulkanBuffer*)vertex_buffer_index);
}

IndexBufferHandle VulkanBackend::createIndexBuffer(void* indices, uint32_t indices_count, IndexType type, MemoryUsage memory_usage)
{
	return (IndexBufferHandle) new VulkanIndexBuffer(this->vulkan->allocator, this->vulkan->graphics_command_pool_set->getPrimaryCommandPool(), this->vulkan->device->getGraphicsQueue(), indices, (indices_count * (type == IndexType::uint16 ? sizeof(uint16_t) : sizeof(uint32_t))), getMemoryUsage(memory_usage), indices_count, type);
}

void VulkanBackend::destroyIndexBuffer(IndexBufferHandle index_buffer_index)
{
	this->vulkan->buffer_deleter->addToQueue((VulkanBuffer*)index_buffer_index);
}

TextureHandle VulkanBackend::createTexture(vector2U size, void* data, uint64_t data_size)
{
	VulkanTexture* texture = new VulkanTexture(this->vulkan->device, this->vulkan->allocator, {size.x, size.y}, getMemoryUsage(MemoryUsage::GPU_Only), this->vulkan->linear_sampler);
	texture->fillTexture(this->vulkan->graphics_command_pool_set->getPrimaryCommandPool(), this->vulkan->device->getGraphicsQueue(), data, data_size);
	return (TextureHandle)texture;
}

void VulkanBackend::destroyTexture(TextureHandle texture_handle)
{
	this->vulkan->texture_deleter->addToQueue((VulkanTexture*)texture_handle);
}

ShaderHandle VulkanBackend::createShader(string vert_data, string frag_data)
{
	VulkanShader* shader = new VulkanShader(this->vulkan->device->get(), vert_data, frag_data);
	return shader;
}

void VulkanBackend::destroyShader(ShaderHandle shader_handle)
{
	this->vulkan->shader_deleter->addToQueue((VulkanShader*)shader_handle);
}

ViewHandle VulkanBackend::createView(vector2U size, FramebufferLayout& layout)
{
	uint32_t frames_in_flight = (uint32_t)this->vulkan->FRAME_COUNT;
	VkExtent2D vk_size = {size.x, size.y};

	Array<VkFormat> color(layout.getColorCount());
	for (size_t i = 0; i < color.size(); i++)
	{
		color[i] = getImageFormat(layout.getColor(i));
	}

	VkFormat depth = getImageFormat(layout.getDepth());

	VkRenderPass render_pass = this->vulkan->render_pass_manager->getRenderPass(layout.getHash(), color, depth);

	VulkanView* view = new VulkanView(this->vulkan->device, this->vulkan->allocator, frames_in_flight, this->vulkan->graphics_command_pool_set, vk_size, color, depth, render_pass);

	/*Array<VkClearValue> clear_values(2);
	clear_values[0].color = { 0.8f, 0.0f, 0.8f, 1.0f };
	clear_values[1].depthStencil = { 1.0f, 0 };
	view->setClearValues(clear_values);*/

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

CommandBuffer* VulkanBackend::getScreenCommandBuffer()
{
	return (CommandBuffer*)this->vulkan->frames_in_flight[this->frame_index].command_buffer_temp;
}

void VulkanBackend::tempDrawScreen(VertexBufferHandle vertices_handle, IndexBufferHandle indices_handle, ShaderHandle shader_handle, TextureHandle texture_handle, UniformBufferHandle uniform_handle)
{
	/*VulkanVertexBuffer* vertices = (VulkanVertexBuffer*)vertices_handle;
	VulkanIndexBuffer* indices = (VulkanIndexBuffer*)indices_handle;
	VulkanShader* shader = (VulkanShader*)shader_handle;

	VulkanTexture* texture = (VulkanTexture*)texture_handle;

	uint32_t thread_index = 0;//Hardcoded for the moment;
	VulkanFrame* frame = &this->vulkan->frames_in_flight[this->frame_index];
	VkCommandBuffer buffer = frame->command_buffer->getSecondaryCommandBuffer(thread_index);

	PipelineSettings settings;

	VulkanPipline* pipeline = this->vulkan->pipeline_manager->getPipeline(shader, this->vulkan->surface->getRenderPass(), settings, vertices->getVertexDescription(), this->vulkan->swapchain->getSwapchainExtent());
	vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());

	VkDescriptorSet descriptor_set = this->vulkan->descriptor_pool->getDescriptorSet(shader->getDescriptorSetLayout());

	VkDescriptorBufferInfo buffer_info = {};
	buffer_info.buffer = uniform->getBuffer(this->frame_index);
	buffer_info.offset = 0;
	buffer_info.range = uniform->getSize();

	uniform->updateBuffer(this->frame_index);

	VkDescriptorImageInfo image_info = {};
	image_info.imageView = texture->getImageView();
	image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image_info.sampler = this->vulkan->linear_sampler;

	Array<VkWriteDescriptorSet> descriptor_set_writes(2);
	descriptor_set_writes[0] = {};
	descriptor_set_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_set_writes[0].dstSet = descriptor_set;
	descriptor_set_writes[0].dstBinding = 0;
	descriptor_set_writes[0].dstArrayElement = 0;
	descriptor_set_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptor_set_writes[0].descriptorCount = 1;
	descriptor_set_writes[0].pBufferInfo = &buffer_info;

	descriptor_set_writes[1] = {};
	descriptor_set_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_set_writes[1].dstSet = descriptor_set;
	descriptor_set_writes[1].dstBinding = 1;
	descriptor_set_writes[1].dstArrayElement = 0;
	descriptor_set_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptor_set_writes[1].descriptorCount = 1;
	descriptor_set_writes[1].pImageInfo = &image_info;

	vkUpdateDescriptorSets(this->vulkan->device->get(), (uint32_t)descriptor_set_writes.size(), descriptor_set_writes.data(), 0, nullptr);

	vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->getPipelineLayout(), 0, 1, &descriptor_set, 0, nullptr);

	VkBuffer vertex_buffer = vertices->get();
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(buffer, 0, 1, &vertex_buffer, &offset);
	vkCmdBindIndexBuffer(buffer, indices->get(), 0, ((indices->getIndicesType() == IndexType::uint16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32)));
	vkCmdDrawIndexed(buffer, indices->getIndicesCount(), 1, 0, 0, 0);*/
}

void VulkanBackend::tempDrawView(ViewHandle view_handle, VertexBufferHandle vertices_handle, IndexBufferHandle indices_handle, ShaderHandle shader_handle, TextureHandle texture_handle, uint32_t index_offset, uint32_t index_count, vector2I scissor_offest, vector2U scissor_extent)
{
	/*VulkanView* view = (VulkanView*)view_handle;
	VulkanVertexBuffer* vertices = (VulkanVertexBuffer*)vertices_handle;
	VulkanIndexBuffer* indices = (VulkanIndexBuffer*)indices_handle;
	VulkanShader* shader = (VulkanShader*)shader_handle;
	VulkanTexture* texture = (VulkanTexture*)texture_handle;

	uint32_t thread_index = 0;//Hardcoded for the moment;
	//VkCommandBuffer buffer = view->getCommandBuffer(this->frame_index)->getSecondaryCommandBuffer(thread_index);
	VulkanFrame* frame = &this->vulkan->frames_in_flight[this->frame_index];
	VkCommandBuffer buffer = frame->command_buffer->getSecondaryCommandBuffer(thread_index);

	PipelineSettings settings;
	settings.depth_test = DepthTest::None;
	settings.cull_mode = CullMode::None;

	VulkanPipline* pipeline = this->vulkan->pipeline_manager->getPipeline(shader, this->vulkan->surface->getRenderPass(), settings, vertices->getVertexDescription(), this->vulkan->swapchain->getSwapchainExtent());
	vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());

	VkDescriptorSet descriptor_set = this->vulkan->descriptor_pool->getDescriptorSet(shader->getDescriptorSetLayout());

	VkDescriptorImageInfo image_info = {};
	image_info.imageView = texture->getImageView();
	image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image_info.sampler = this->vulkan->linear_sampler;

	Array<VkWriteDescriptorSet> descriptor_set_writes(1);
	descriptor_set_writes[0] = {};
	descriptor_set_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_set_writes[0].dstSet = descriptor_set;
	descriptor_set_writes[0].dstBinding = 0;
	descriptor_set_writes[0].dstArrayElement = 0;
	descriptor_set_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptor_set_writes[0].descriptorCount = 1;
	descriptor_set_writes[0].pImageInfo = &image_info;

	vkUpdateDescriptorSets(this->vulkan->device->get(), (uint32_t)descriptor_set_writes.size(), descriptor_set_writes.data(), 0, nullptr);

	VkRect2D scissor = { {scissor_offest.x, scissor_offest.y}, {scissor_extent.x, scissor_extent.y}};
	//vkCmdSetScissor(buffer, 0, 1, &scissor);

	vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->getPipelineLayout(), 0, 1, &descriptor_set, 0, nullptr);

	VkBuffer vertex_buffer = vertices->get();
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(buffer, 0, 1, &vertex_buffer, &offset);
	vkCmdBindIndexBuffer(buffer, indices->get(), 0, VK_INDEX_TYPE_UINT16);
	vkCmdDrawIndexed(buffer, index_count, 1, index_offset, 0, 0);*/

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

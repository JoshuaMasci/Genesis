#include "VulkanCommandBuffer.hpp"

#include "Genesis/Rendering/Vulkan/VulkanTexture.hpp"
#include "Genesis/Rendering/Vulkan/VulkanView.hpp"

using namespace Genesis;

VulkanCommandBuffer::VulkanCommandBuffer(uint32_t thread_index, uint32_t frame_index, VkDevice device, VulkanCommandPool* command_pool, VulkanPipelineManager* pipeline_manager, VulkanDescriptorPool* descriptor_pool, VulkanUniformPool* uniform_pool, VkSampler temp_sampler)
	:thread_index(thread_index),
	frame_index(frame_index)
{
	this->device = device;
	this->command_pool = command_pool;
	this->command_buffer = this->command_pool->getCommandBuffer();
	this->pipeline_manager = pipeline_manager;
	this->descriptor_pool = descriptor_pool;
	this->uniform_pool = uniform_pool;

	this->sampler = temp_sampler;
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
	this->command_pool->freeCommandBuffer(this->command_buffer);
}

void VulkanCommandBuffer::beginCommandBuffer(VkRenderPassBeginInfo& render_pass_info, VkSubpassContents mode)
{
	this->render_pass = render_pass_info.renderPass;

	vkResetCommandBuffer(this->command_buffer, 0);

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	if (vkBeginCommandBuffer(this->command_buffer, &begin_info) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	vkCmdBeginRenderPass(this->command_buffer, &render_pass_info, mode);
}

void VulkanCommandBuffer::endCommandBuffer()
{
	vkCmdEndRenderPass(this->command_buffer);
	vkEndCommandBuffer(this->command_buffer);
}

void VulkanCommandBuffer::submitCommandBuffer(VkQueue queue, Array<VkSemaphore>& wait_semaphores, Array<VkPipelineStageFlags>& wait_states, Array<VkSemaphore>& signal_semaphores, VkFence trigger_fence)
{
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	submit_info.waitSemaphoreCount = (uint32_t)wait_semaphores.size();
	submit_info.pWaitSemaphores = wait_semaphores.data();
	submit_info.pWaitDstStageMask = wait_states.data();

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &this->command_buffer;

	submit_info.signalSemaphoreCount = (uint32_t)signal_semaphores.size();
	submit_info.pSignalSemaphores = signal_semaphores.data();

	if (vkQueueSubmit(queue, 1, &submit_info, trigger_fence) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit command buffer!");
	}

	this->setShader(nullptr);
}

void VulkanCommandBuffer::setUniform(string name, void* data, uint32_t data_size)
{
	assert(this->current_shader != nullptr);

	ShaderBindingVariableLocation location = this->current_shader->getVariableLocation(name);

	if (data_size != location.variable_size)
	{
		printf("Warning: Size mismatch on uniform data: %s\n", name.c_str());
	}

	memcpy_s(this->binding_uniform_buffers[location.binding].data() + location.variable_offset, location.variable_size, data, data_size);
}

void VulkanCommandBuffer::setShader(ShaderHandle shader)
{
	//Clear old uniforms
	this->binding_uniform_buffers.clear();
	this->binding_image.clear();

	this->current_shader = (VulkanShader*)shader;

	if (this->current_shader != nullptr)
	{
		for (auto binding : this->current_shader->name_bindings)
		{
			if (binding.second->type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
			{
				this->binding_image[binding.second->binding_location] = VK_NULL_HANDLE;
			}
			else
			{
				//Create the staging buffer
				uint32_t binding_index = binding.second->binding_location;
				uint32_t binding_size = binding.second->total_size;
				this->binding_uniform_buffers[binding_index] = Array<uint8_t>(binding_size);
			}
		}
	}
}

void VulkanCommandBuffer::setPipelineSettings(PipelineSettings& settings)
{
	this->current_settings = settings;
}

void VulkanCommandBuffer::setScissor(vector2U offset, vector2U extent)
{
	VkRect2D rect = {};
	rect.offset = {(int32_t)offset.x, (int32_t)offset.y};
	rect.extent = {extent.x, extent.y};
	vkCmdSetScissor(this->command_buffer, 0, 1, &rect);
}

void VulkanCommandBuffer::setUniformFloat(string name, float value)
{
	this->setUniform(name, &value, sizeof(float));
}

void VulkanCommandBuffer::setUniformVec4(string name, vector4F value)
{
	this->setUniform(name, &value, sizeof(vector4F));
}

void VulkanCommandBuffer::setUniformMat4(string name, matrix4F value)
{
	this->setUniform(name, &value, sizeof(matrix4F));
}

void VulkanCommandBuffer::setUniformTexture(string name, TextureHandle texture)
{
	assert(this->current_shader != nullptr);
	ShaderBindingVariableLocation location = this->current_shader->getVariableLocation(name);
	this->binding_image[location.binding] = ((VulkanTexture*)texture)->getImageView();
}

void VulkanCommandBuffer::setUniformView(string name, ViewHandle view, uint16_t view_image_index)
{
	assert(this->current_shader != nullptr);
	ShaderBindingVariableLocation location = this->current_shader->getVariableLocation(name);
	this->binding_image[location.binding] = ((VulkanView*)view)->getFramebuffer(this->frame_index)->getImageView(view_image_index);
}

void VulkanCommandBuffer::drawIndexed(VertexBufferHandle vertex_handle, IndexBufferHandle index_handle)
{
	this->drawIndexedOffset(vertex_handle, index_handle, 0, ((VulkanIndexBuffer*)index_handle)->getIndicesCount());
}

void VulkanCommandBuffer::drawIndexedOffset(VertexBufferHandle vertex_handle, IndexBufferHandle index_handle, uint32_t index_offset, uint32_t index_count)
{
	VulkanVertexBuffer* vertex_buffer = (VulkanVertexBuffer*)vertex_handle;
	VulkanIndexBuffer* index_buffer = (VulkanIndexBuffer*)index_handle;

	VulkanPipline* pipeline = this->pipeline_manager->getPipeline(this->current_shader, this->render_pass, this->current_settings, vertex_buffer->getVertexDescription(), this->size);

	if (this->current_pipeline != pipeline)
	{
		vkCmdBindPipeline(this->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());
		this->current_pipeline = pipeline;
	}

	if ((this->binding_uniform_buffers.size() + this->binding_image.size()) > 0)
	{
		VkDescriptorSet descriptor_set = this->descriptor_pool->getDescriptorSet(this->current_shader->getDescriptorSetLayout(), this->frame_index);

		Array<VkWriteDescriptorSet> write_descriptors(this->binding_uniform_buffers.size() + this->binding_image.size());
		size_t write_index = 0;

		vector<VkDescriptorBufferInfo> buffer_info_list;
		for (auto buffer : this->binding_uniform_buffers)
		{
			Array<uint8_t>* temp_buffer = &buffer.second;
			VulkanBuffer* uniform = this->uniform_pool->getBuffer(this->frame_index, temp_buffer->size());
			uniform->fillBufferHostVisable(temp_buffer->data(), temp_buffer->size());

			VkWriteDescriptorSet& descriptor_write = write_descriptors[write_index];
			write_index++;

			size_t info_index = buffer_info_list.size();
			buffer_info_list.push_back({});
			buffer_info_list[info_index].buffer = uniform->get();
			buffer_info_list[info_index].offset = 0;
			buffer_info_list[info_index].range = (VkDeviceSize)uniform->getSize();

			descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_write.pNext = VK_NULL_HANDLE;
			descriptor_write.dstSet = descriptor_set;
			descriptor_write.dstBinding = buffer.first;
			descriptor_write.dstArrayElement = 0;
			descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptor_write.descriptorCount = 1;
			descriptor_write.pBufferInfo = &buffer_info_list[info_index];
			descriptor_write.pImageInfo = nullptr;
			descriptor_write.pTexelBufferView = nullptr;
		}

		vector<VkDescriptorImageInfo> image_info_list;
		for (auto image : this->binding_image)
		{
			VkWriteDescriptorSet& descriptor_write = write_descriptors[write_index];
			write_index++;

			size_t info_index = image_info_list.size();
			image_info_list.push_back({});
			image_info_list[info_index].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			image_info_list[info_index].imageView = image.second;
			image_info_list[info_index].sampler = this->sampler;

			descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_write.pNext = VK_NULL_HANDLE;
			descriptor_write.dstSet = descriptor_set;
			descriptor_write.dstBinding = image.first;
			descriptor_write.dstArrayElement = 0;
			descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptor_write.descriptorCount = 1;
			descriptor_write.pBufferInfo = nullptr;
			descriptor_write.pImageInfo = &image_info_list[info_index];
			descriptor_write.pTexelBufferView = nullptr;
		}

		vkUpdateDescriptorSets(this->device, (uint32_t)write_descriptors.size(), write_descriptors.data(), 0, nullptr);

		vkCmdBindDescriptorSets(this->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->current_shader->getPipelineLayout(), 0, 1, &descriptor_set, 0, nullptr);
	}

	VkBuffer vertexBuffers[] = { vertex_buffer->get() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(this->command_buffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(this->command_buffer, index_buffer->get(), 0, (index_buffer->getIndicesType() == IndexType::uint16) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(this->command_buffer, index_count, 1, index_offset, 0, 0);
}

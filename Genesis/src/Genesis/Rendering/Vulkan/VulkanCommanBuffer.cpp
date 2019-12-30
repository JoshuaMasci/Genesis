#include "VulkanCommanBuffer.hpp"

#include "Genesis/Rendering/Vulkan/VulkanShader.hpp"
#include "Genesis/Rendering/Vulkan/VulkanBuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanTexture.hpp"
#include "Genesis/Rendering/Vulkan/VulkanFramebuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanView.hpp"

using namespace Genesis;

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device, VulkanCommandPool* command_pool, VulkanThreadPipelinePool* pipeline_pool, VulkanDescriptorPool* descriptor_pool, uint32_t frame_index)
	:FRAME_INDEX(frame_index)
{
	this->device = device;
	this->command_pool = command_pool;
	this->pipeline_pool = pipeline_pool;
	this->descriptor_pool = descriptor_pool;

	this->command_buffer = this->command_pool->getCommandBuffer();
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
	this->command_pool->freeCommandBuffer(this->command_buffer);
}

void VulkanCommandBuffer::startPrimary(VkFramebuffer framebuffer, VkRenderPass render_pass, VkRect2D render_area, List<VkClearValue>& clear_values, VkSubpassContents content)
{
	vkResetCommandBuffer(this->command_buffer, 0);

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	if (vkBeginCommandBuffer(this->command_buffer, &begin_info) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	//Setup Default Dynamic States
	VkViewport viewport = {};
	viewport.x = (float)render_area.offset.x;
	viewport.y = (float)render_area.offset.y;
	viewport.width = (float)render_area.extent.width;
	viewport.height = (float)render_area.extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(this->command_buffer, 0, 1, &viewport);
	vkCmdSetScissor(this->command_buffer, 0, 1, &render_area);

	VkRenderPassBeginInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.framebuffer = framebuffer;
	render_pass_info.renderPass = render_pass;
	render_pass_info.renderArea = render_area;
	render_pass_info.clearValueCount = (uint32_t)clear_values.size();
	render_pass_info.pClearValues = clear_values.data();

	vkCmdBeginRenderPass(this->command_buffer, &render_pass_info, content);

	this->current_framebuffer = framebuffer;
	this->current_render_pass = render_pass;
}

void VulkanCommandBuffer::startSecondary(VkFramebuffer framebuffer, VkRenderPass render_pass)
{
	vkResetCommandBuffer(this->command_buffer, 0);

	VkCommandBufferInheritanceInfo inheritance_info = {};
	inheritance_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	inheritance_info.framebuffer = framebuffer;
	inheritance_info.renderPass = render_pass;

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.pInheritanceInfo = &inheritance_info;
	if (vkBeginCommandBuffer(this->command_buffer, &begin_info) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	this->current_framebuffer = framebuffer;
	this->current_render_pass = render_pass;
}

void VulkanCommandBuffer::end()
{
	vkCmdEndRenderPass(this->command_buffer);
	vkEndCommandBuffer(this->command_buffer);

	//Reset
	this->current_framebuffer = VK_NULL_HANDLE;
	this->current_render_pass = VK_NULL_HANDLE;

	this->current_shader = nullptr;
	this->current_descriptor_sets.clear();

	this->current_vertex_description = nullptr;

	this->current_pipeline_layout = VK_NULL_HANDLE;
	this->current_pipeline = VK_NULL_HANDLE;
}

void VulkanCommandBuffer::submit(VkQueue queue, List<VkSemaphore>& wait_semaphores, List<VkPipelineStageFlags>& wait_states, List<VkSemaphore>& signal_semaphores, VkFence trigger_fence)
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
}

void VulkanCommandBuffer::executeSecondary(List<VkCommandBuffer>& secondary_command_buffers)
{
	vkCmdExecuteCommands(this->command_buffer, (uint32_t)secondary_command_buffers.size(), secondary_command_buffers.data());
}

void VulkanCommandBuffer::setShader(VulkanShader* new_shader)
{
	if (this->current_shader == new_shader)
	{
		return; //Do nothing
	}

	this->current_shader = new_shader;

	if (this->current_pipeline_layout == new_shader->getPipelineLayout())
	{
		return; //Do nothing
	}

	auto new_descriptor_layouts = new_shader->getDescriptorSetLayouts();
	auto new_descriptor_bindings = new_shader->getDescriptorSetBindings();

	this->current_descriptor_sets.resize(new_descriptor_bindings.size());
	for (size_t set_index = 0; set_index < this->current_descriptor_sets.size(); set_index++)
	{
		if (this->current_descriptor_sets[set_index].layout == new_descriptor_layouts[set_index])
		{
			continue;
		}

		this->current_descriptor_sets[set_index].layout = new_descriptor_layouts[set_index];
		this->current_descriptor_sets[set_index].last_set = VK_NULL_HANDLE;
		this->current_descriptor_sets[set_index].bindings.resize(new_descriptor_bindings[set_index].size());

		for (size_t binding_index = 0; binding_index < this->current_descriptor_sets[set_index].bindings.size(); binding_index++)
		{
			this->current_descriptor_sets[set_index].bindings[binding_index].type = new_descriptor_bindings[set_index][binding_index].type;

			if (this->current_descriptor_sets[set_index].bindings[binding_index].type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
			{
				this->current_descriptor_sets[set_index].bindings[binding_index].data.uniform_buffer.size = new_descriptor_bindings[set_index][binding_index].size;
			}
		}

		this->current_descriptor_sets[set_index].has_changed = true;
	}
}

void VulkanCommandBuffer::setPipelineSettings(PipelineSettings& settings)
{
	this->current_pipeline_settings = settings;
}

void VulkanCommandBuffer::setScissor(VkRect2D& scissor_rect)
{
	vkCmdSetScissor(this->command_buffer, 0, 1, &scissor_rect);
}

void VulkanCommandBuffer::setUniformBuffer(uint32_t set, uint32_t binding, VkBuffer buffer, uint64_t buffer_size)
{
	assert(set < this->current_descriptor_sets.size() && binding < this->current_descriptor_sets[set].bindings.size());
	assert(this->current_descriptor_sets[set].bindings[binding].type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

	this->current_descriptor_sets[set].bindings[binding].data.uniform_buffer.buffer = buffer;
	this->current_descriptor_sets[set].bindings[binding].data.uniform_buffer.size = buffer_size;
}

void VulkanCommandBuffer::setUniformTexture(uint32_t set, uint32_t binding, VkImageView texture, VkSampler sampler)
{
	assert(set < this->current_descriptor_sets.size() && binding < this->current_descriptor_sets[set].bindings.size());
	assert(this->current_descriptor_sets[set].bindings[binding].type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

	this->current_descriptor_sets[set].bindings[binding].data.image_sampler.image = texture;
	this->current_descriptor_sets[set].bindings[binding].data.image_sampler.sampler = sampler;
}

void VulkanCommandBuffer::setUniformConstant(void* data, uint32_t data_size)
{
	vkCmdPushConstants(this->command_buffer, this->current_pipeline_layout, VK_SHADER_STAGE_ALL, 0, data_size, data);
}

void VulkanCommandBuffer::setVertexBuffer(VkBuffer vertex_buffer, VertexInputDescription* vertex_description)
{
	this->current_vertex_description = vertex_description;

	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(this->command_buffer, 0, 1, &vertex_buffer, offsets);
}

void VulkanCommandBuffer::setIndexBuffer(VkBuffer index_buffer, IndexType type)
{
	vkCmdBindIndexBuffer(this->command_buffer, index_buffer, 0, (type == IndexType::uint16) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
}

void VulkanCommandBuffer::drawIndexed(uint32_t index_count, uint32_t index_offset, uint32_t instance_count, uint32_t instance_offset)
{
	this->bindDescriptors();

	VkPipeline pipeline = this->pipeline_pool->getPipeline(this->current_shader, this->current_render_pass, this->current_pipeline_settings, this->current_vertex_description);

	if (this->current_pipeline != pipeline)
	{
		vkCmdBindPipeline(this->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		this->current_pipeline = pipeline;
	}

	vkCmdDrawIndexed(this->command_buffer, index_count, instance_count, index_offset, 0, instance_offset);
}

void VulkanCommandBuffer::bindDescriptors()
{
	for (size_t set_index = 0; set_index < this->current_descriptor_sets.size(); set_index++)
	{
		DescriptorSet& set = this->current_descriptor_sets[set_index];
		if (set.has_changed == true)
		{
			set.last_set = this->descriptor_pool->getDescriptorSet(set.layout, this->FRAME_INDEX);

			size_t binding_count = set.bindings.size();
			List<VkWriteDescriptorSet> descriptor_info(binding_count);
			List<VkDescriptorBufferInfo> buffer_info(binding_count);
			List<VkDescriptorImageInfo> image_sampler_info(binding_count);

			for (size_t i = 0; i < binding_count; i++)
			{
				DescriptorBinding& binding = set.bindings[i];

				if (binding.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				{
					buffer_info[i].buffer = binding.data.uniform_buffer.buffer;
					buffer_info[i].offset = 0;
					buffer_info[i].range = (VkDeviceSize)binding.data.uniform_buffer.size;

					descriptor_info[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptor_info[i].pNext = VK_NULL_HANDLE;
					descriptor_info[i].dstSet = set.last_set;
					descriptor_info[i].dstBinding = (uint32_t)i;
					descriptor_info[i].dstArrayElement = 0;
					descriptor_info[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					descriptor_info[i].descriptorCount = 1;
					descriptor_info[i].pBufferInfo = &buffer_info[i];
					descriptor_info[i].pImageInfo = nullptr;
					descriptor_info[i].pTexelBufferView = nullptr;
				}
				else if (binding.type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
				{
					image_sampler_info[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					image_sampler_info[i].imageView = binding.data.image_sampler.image;
					image_sampler_info[i].sampler = binding.data.image_sampler.sampler;

					descriptor_info[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptor_info[i].pNext = VK_NULL_HANDLE;
					descriptor_info[i].dstSet = set.last_set;
					descriptor_info[i].dstBinding = (uint32_t)i;
					descriptor_info[i].dstArrayElement = 0;
					descriptor_info[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptor_info[i].descriptorCount = 1;
					descriptor_info[i].pBufferInfo = nullptr;
					descriptor_info[i].pImageInfo = &image_sampler_info[i];
					descriptor_info[i].pTexelBufferView = nullptr;
				}
			}
			vkUpdateDescriptorSets(this->device->get(), (uint32_t)descriptor_info.size(), descriptor_info.data(), 0, nullptr);
			set.has_changed = false;
		}

		vkCmdBindDescriptorSets(this->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->current_pipeline_layout, 0, 1, &set.last_set, 0, nullptr);
	}
}

VulkanCommandBufferSingle::VulkanCommandBufferSingle(VulkanDevice* device, VulkanCommandPool* command_pool, VulkanThreadPipelinePool* pipeline_pool, VulkanDescriptorPool* descriptor_pool, uint32_t frame_index)
	:command_buffer(device, command_pool, pipeline_pool, descriptor_pool, frame_index)
{
}

VulkanCommandBufferSingle::~VulkanCommandBufferSingle()
{
}

void VulkanCommandBufferSingle::setShader(Shader shader)
{
	this->command_buffer.setShader((VulkanShader*)shader);
}

void VulkanCommandBufferSingle::setPipelineSettings(PipelineSettings& settings)
{
	this->command_buffer.setPipelineSettings(settings);
}

void VulkanCommandBufferSingle::setScissor(vector2I offset, vector2U extent)
{
	VkRect2D rect = {};
	rect.offset = { offset.x, offset.y };
	rect.extent = { extent.x, extent.y };
	this->command_buffer.setScissor(rect);
}

void VulkanCommandBufferSingle::setUniformBuffer(uint32_t set, uint32_t binding, UniformBuffer buffer)
{
	VulkanBuffer* uniform = ((VulkanUniformBuffer*)buffer)->getCurrentBuffer();

	this->command_buffer.setUniformBuffer(set, binding, uniform->get(), uniform->getSize());
}

void VulkanCommandBufferSingle::setUniformTexture(uint32_t set, uint32_t binding, Texture texture)
{
	this->command_buffer.setUniformTexture(set, binding, ((VulkanTexture*)texture)->getImageView(), VK_NULL_HANDLE);
}

void VulkanCommandBufferSingle::setUniformView(uint32_t set, uint32_t binding, View view, uint8_t view_image_index)
{
	VulkanViewSingleThread* vulkan_view = (VulkanViewSingleThread*)view;
	VkImageView image_view = vulkan_view->getFramebuffer(this->command_buffer.getFrameIndex())->getImageView(view_image_index);
	this->command_buffer.setUniformTexture(set, binding, image_view, VK_NULL_HANDLE);
}

void VulkanCommandBufferSingle::setUniformConstant(void* data, uint32_t data_size)
{
	this->command_buffer.setUniformConstant(data, data_size);
}

void VulkanCommandBufferSingle::setVertexBuffer(VertexBuffer vertex, VertexInputDescription& vertex_description)
{
	VulkanVertexBuffer* vertex_buffer = (VulkanVertexBuffer*)vertex;
	this->command_buffer.setVertexBuffer(vertex_buffer->get(), &(vertex_buffer->getVertexDescription()));
}

void VulkanCommandBufferSingle::setIndexBuffer(IndexBuffer index, IndexType type)
{
	VulkanIndexBuffer* index_buffer = (VulkanIndexBuffer*)index;
	this->command_buffer.setIndexBuffer(index_buffer->get(), index_buffer->getIndicesType());
}

void VulkanCommandBufferSingle::drawIndexed(uint32_t index_count, uint32_t index_offset, uint32_t instance_count, uint32_t instance_offset)
{
	this->command_buffer.drawIndexed(index_count, index_offset, instance_count, instance_offset);
}
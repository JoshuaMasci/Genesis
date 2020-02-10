#include "VulkanCommanBuffer.hpp"

#include "Genesis/Debug/Assert.hpp"

#include "VulkanShader.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanTexture.hpp"
#include "VulkanFramebuffer.hpp"

using namespace Genesis;

VulkanCommandBufferInternal::VulkanCommandBufferInternal(VulkanDevice* device, VulkanCommandPool* command_pool, VulkanThreadPipelinePool* pipeline_pool, VulkanDescriptorPool* descriptor_pool, uint32_t frame_index)
	:FRAME_INDEX(frame_index)
{
	this->device = device;
	this->command_pool = command_pool;
	this->pipeline_pool = pipeline_pool;
	this->descriptor_pool = descriptor_pool;

	this->command_buffer = this->command_pool->getCommandBuffer();
}

VulkanCommandBufferInternal::~VulkanCommandBufferInternal()
{
	this->command_pool->freeCommandBuffer(this->command_buffer);
}

void VulkanCommandBufferInternal::startPrimary(VkFramebuffer frameBuffer, VkRenderPass render_pass, VkRect2D render_area, List<VkClearValue>& clear_values, VkSubpassContents content)
{
	vkResetCommandBuffer(this->command_buffer, 0);

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	GENESIS_ENGINE_ASSERT_ERROR(vkBeginCommandBuffer(this->command_buffer, &begin_info) == VK_SUCCESS, "failed to begin recording command buffer");

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
	render_pass_info.framebuffer = frameBuffer;
	render_pass_info.renderPass = render_pass;
	render_pass_info.renderArea = render_area;
	render_pass_info.clearValueCount = (uint32_t)clear_values.size();
	render_pass_info.pClearValues = clear_values.data();

	vkCmdBeginRenderPass(this->command_buffer, &render_pass_info, content);

	this->current_framebuffer = frameBuffer;
	this->current_render_pass = render_pass;
}

void VulkanCommandBufferInternal::startSecondary(VkFramebuffer frameBuffer, VkRenderPass render_pass, VkRect2D render_area)
{
	vkResetCommandBuffer(this->command_buffer, 0);

	VkCommandBufferInheritanceInfo inheritance_info = {};
	inheritance_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	inheritance_info.framebuffer = frameBuffer;
	inheritance_info.renderPass = render_pass;

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.pInheritanceInfo = &inheritance_info;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

	GENESIS_ENGINE_ASSERT_ERROR(vkBeginCommandBuffer(this->command_buffer, &begin_info) == VK_SUCCESS, "failed to begin recording command buffer");

	this->current_framebuffer = frameBuffer;
	this->current_render_pass = render_pass;

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
}

void VulkanCommandBufferInternal::endPrimary()
{
	vkCmdEndRenderPass(this->command_buffer);
	this->endSecondary();

	this->current_framebuffer = VK_NULL_HANDLE;
	this->current_render_pass = VK_NULL_HANDLE;

	this->current_shader = nullptr;
	this->current_descriptor_sets.clear();

	this->current_vertex_description = nullptr;

	this->current_pipeline_layout = VK_NULL_HANDLE;
	this->current_pipeline = VK_NULL_HANDLE;
}

void VulkanCommandBufferInternal::endSecondary()
{
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

void VulkanCommandBufferInternal::setShader(VulkanShader* new_shader)
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

	this->current_pipeline_layout = new_shader->getPipelineLayout();

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

void VulkanCommandBufferInternal::setPipelineSettings(PipelineSettings& settings)
{
	this->current_pipeline_settings = settings;
}

void VulkanCommandBufferInternal::setScissor(VkRect2D& scissor_rect)
{
	vkCmdSetScissor(this->command_buffer, 0, 1, &scissor_rect);
}

void VulkanCommandBufferInternal::setUniformBuffer(uint32_t set, uint32_t binding, VkBuffer buffer, uint64_t buffer_size)
{
	GENESIS_ENGINE_ASSERT_ERROR((set < this->current_descriptor_sets.size() && binding < this->current_descriptor_sets[set].bindings.size()), "Binding index out of range");
	GENESIS_ENGINE_ASSERT_ERROR((this->current_descriptor_sets[set].bindings[binding].type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER), "Incorrect binding type");

	this->current_descriptor_sets[set].bindings[binding].data.uniform_buffer.buffer = buffer;
	this->current_descriptor_sets[set].bindings[binding].data.uniform_buffer.size = buffer_size;
	this->current_descriptor_sets[set].has_changed = true;
}

void VulkanCommandBufferInternal::setUniformTexture(uint32_t set, uint32_t binding, VkImageView texture, VkSampler sampler)
{
	GENESIS_ENGINE_ASSERT_ERROR((set < this->current_descriptor_sets.size() && binding < this->current_descriptor_sets[set].bindings.size()), "Binding index out of range");
	GENESIS_ENGINE_ASSERT_ERROR((this->current_descriptor_sets[set].bindings[binding].type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER), "Incorrect binding type");

	this->current_descriptor_sets[set].bindings[binding].data.image_sampler.image = texture;
	this->current_descriptor_sets[set].bindings[binding].data.image_sampler.sampler = sampler;
	this->current_descriptor_sets[set].has_changed = true;
}

void VulkanCommandBufferInternal::setUniformConstant(void* data, uint32_t data_size)
{
	vkCmdPushConstants(this->command_buffer, this->current_pipeline_layout, VK_SHADER_STAGE_ALL, 0, data_size, data); //TODO use specific stage flags
}

void VulkanCommandBufferInternal::setVertexBuffer(VkBuffer vertex_buffer, VulkanVertexInputDescription* vertex_description)
{
	this->current_vertex_description = vertex_description;

	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(this->command_buffer, 0, 1, &vertex_buffer, offsets);
}

void VulkanCommandBufferInternal::setIndexBuffer(VkBuffer index_buffer, VkIndexType type)
{
	vkCmdBindIndexBuffer(this->command_buffer, index_buffer, 0, type);
}

void VulkanCommandBufferInternal::drawIndexed(uint32_t index_count, uint32_t index_offset, uint32_t instance_count, uint32_t instance_offset)
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

void VulkanCommandBufferInternal::bindDescriptors()
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

		vkCmdBindDescriptorSets(this->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->current_pipeline_layout, set_index, 1, &set.last_set, 0, nullptr);
	}
}

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device, VulkanCommandPool* command_pool, VulkanThreadPipelinePool* pipeline_pool, VulkanDescriptorPool* descriptor_pool, VulkanTransferBuffer* transfer_buffer, uint32_t frame_index)
	:command_buffer(device, command_pool, pipeline_pool, descriptor_pool, frame_index)
{
	this->transfer_buffer = transfer_buffer;
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
}

void VulkanCommandBuffer::startPrimary(VkFramebuffer frameBuffer, VkRenderPass render_pass, VkRect2D render_area, List<VkClearValue>& clear_values, VkSubpassContents content)
{
	this->command_buffer.startPrimary(frameBuffer, render_pass, render_area, clear_values, content);
}

void VulkanCommandBuffer::startSecondary(VkFramebuffer frameBuffer, VkRenderPass render_pass, VkRect2D render_area)
{
	this->command_buffer.startSecondary(frameBuffer, render_pass, render_area);
}

void VulkanCommandBuffer::endPrimary()
{
	this->command_buffer.endPrimary();
}

void VulkanCommandBuffer::endSecondary()
{
	this->command_buffer.endSecondary();
}

void VulkanCommandBuffer::setShader(Shader shader)
{
	this->command_buffer.setShader((VulkanShader*)shader);
}

void VulkanCommandBuffer::setPipelineSettings(PipelineSettings& settings)
{
	this->command_buffer.setPipelineSettings(settings);
}

void VulkanCommandBuffer::setScissor(vector2I offset, vector2U extent)
{
	VkRect2D rect = {};
	rect.offset = { offset.x, offset.y };
	rect.extent = { extent.x, extent.y };
	this->command_buffer.setScissor(rect);
}

void VulkanCommandBuffer::setUniformBuffer(uint32_t set, uint32_t binding, DynamicBuffer uniform_buffer)
{
	VulkanBuffer* uniform = ((VulkanDynamicBuffer*)uniform_buffer)->getCurrentBuffer();
	this->command_buffer.setUniformBuffer(set, binding, uniform->get(), uniform->getSize());
}

void VulkanCommandBuffer::setUniformTexture(uint32_t set, uint32_t binding, Texture texture, Sampler sampler)
{
	this->command_buffer.setUniformTexture(set, binding, ((VulkanTexture*)texture)->getImageView(), (VkSampler)sampler);
}

void VulkanCommandBuffer::setUniformFramebuffer(uint32_t set, uint32_t binding, Framebuffer framebuffer, uint8_t framebuffer_image_index, Sampler sampler)
{
	VulkanFramebufferSet* vulkan_framebuffer = (VulkanFramebufferSet*)framebuffer;

	VkImageView image_view = vulkan_framebuffer->framebuffers[this->command_buffer.getFrameIndex()]->getImageView(framebuffer_image_index);
	this->command_buffer.setUniformTexture(set, binding, image_view, (VkSampler)sampler);
}

void VulkanCommandBuffer::setUniformConstant(void* data, uint32_t data_size)
{
	this->command_buffer.setUniformConstant(data, data_size);
}

void VulkanCommandBuffer::setVertexBuffer(StaticBuffer vertex_buffer, VertexInputDescription& vertex_description)
{
	this->command_buffer.setVertexBuffer(((VulkanBuffer*)vertex_buffer)->get(), (VulkanVertexInputDescription*)vertex_description);
}

void VulkanCommandBuffer::setIndexBuffer(StaticBuffer index_buffer, IndexType type)
{
	this->command_buffer.setIndexBuffer(((VulkanBuffer*)index_buffer)->get(), (type == IndexType::uint16) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
}

void VulkanCommandBuffer::drawIndexed(uint32_t index_count, uint32_t index_offset, uint32_t instance_count, uint32_t instance_offset)
{
	this->command_buffer.drawIndexed(index_count, index_offset, instance_count, instance_offset);
}

VulkanCommandBufferMultithread::VulkanCommandBufferMultithread(VulkanDevice* device, uint32_t thread_count, VulkanCommandPool* primary_pool, List<VulkanCommandPool*> secondary_pools, List<VulkanThreadPipelinePool*> pipeline_pools, List<VulkanDescriptorPool*> descriptor_pools, VulkanTransferBuffer* transfer_buffer, uint32_t frame_index)
{
	this->primary_pool = primary_pool;
	this->primary_buffer = this->primary_pool->getCommandBuffer();

	this->secondary_buffers.resize(thread_count);
	for (size_t i = 0; i < this->secondary_buffers.size(); i++)
	{
		this->secondary_buffers[i] = new VulkanCommandBuffer(device, secondary_pools[i], pipeline_pools[i], descriptor_pools[i], transfer_buffer, frame_index);
	}
}

VulkanCommandBufferMultithread::~VulkanCommandBufferMultithread()
{
	this->primary_pool->freeCommandBuffer(this->primary_buffer);

	for (size_t i = 0; i < this->secondary_buffers.size(); i++)
	{
		delete this->secondary_buffers[i];
	}
}

void VulkanCommandBufferMultithread::start(VulkanFramebuffer* framebuffer_target)
{
	vkResetCommandBuffer(this->primary_buffer, 0);

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	GENESIS_ENGINE_ASSERT_ERROR(vkBeginCommandBuffer(this->primary_buffer, &begin_info) == VK_SUCCESS, "failed to begin recording command buffer");

	VkFramebuffer frameBuffer = framebuffer_target->get();
	VkRenderPass render_pass = framebuffer_target->getRenderPass();
	VkRect2D render_area = { {0, 0}, framebuffer_target->getSize() };

	//TODO clear values
	List<VkClearValue>& clear_values = framebuffer_target->getClearValues();

	//Setup Default Dynamic States
	VkViewport viewport = {};
	viewport.x = (float)render_area.offset.x;
	viewport.y = (float)render_area.offset.y;
	viewport.width = (float)render_area.extent.width;
	viewport.height = (float)render_area.extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(this->primary_buffer, 0, 1, &viewport);
	vkCmdSetScissor(this->primary_buffer, 0, 1, &render_area);

	VkRenderPassBeginInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.framebuffer = frameBuffer;
	render_pass_info.renderPass = render_pass;
	render_pass_info.renderArea = render_area;
	render_pass_info.clearValueCount = (uint32_t)clear_values.size();
	render_pass_info.pClearValues = clear_values.data();

	vkCmdBeginRenderPass(this->primary_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	for (size_t i = 0; i < this->secondary_buffers.size(); i++)
	{
		this->secondary_buffers[i]->startSecondary(frameBuffer, render_pass, render_area);
	}
}

void VulkanCommandBufferMultithread::end()
{
	List<VkCommandBuffer> secondary(this->secondary_buffers.size());
	for (size_t i = 0; i < this->secondary_buffers.size(); i++)
	{
		this->secondary_buffers[i]->endSecondary();
		secondary[i] = this->secondary_buffers[i]->getCommandBuffer();
	}

	vkCmdExecuteCommands(this->primary_buffer, (uint32_t)secondary.size(), secondary.data());

	vkCmdEndRenderPass(this->primary_buffer);
	vkEndCommandBuffer(this->primary_buffer);
}

VulkanCommandBufferMultithreadSet::VulkanCommandBufferMultithreadSet(VulkanDevice* device, uint32_t frame_count, uint32_t thread_count, VulkanCommandPool* primary_pool, List<VulkanCommandPool*> secondary_pools, List<VulkanThreadPipelinePool*> pipeline_pools, List<VulkanDescriptorPool*> descriptor_pools, List<VulkanTransferBuffer*> transfer_buffers)
{
	this->command_buffers.resize(frame_count);
	for (size_t i = 0; i < this->command_buffers.size(); i++)
	{
		this->command_buffers[i] = new VulkanCommandBufferMultithread(device, thread_count, primary_pool, secondary_pools, pipeline_pools, descriptor_pools, transfer_buffers[i], (uint32_t)i);
	}
}

VulkanCommandBufferMultithreadSet::~VulkanCommandBufferMultithreadSet()
{
	for (size_t i = 0; i < this->command_buffers.size(); i++)
	{
		delete this->command_buffers[i];
	}
}

VulkanCommandBufferSet::VulkanCommandBufferSet(VulkanDevice* device, VulkanCommandPool* command_pool, VulkanThreadPipelinePool* pipeline_pool, VulkanDescriptorPool* descriptor_pool, VulkanTransferBuffer* transfer_buffer, uint32_t frame_count)
{
	this->command_buffers.resize(frame_count);
	for (size_t i = 0; i < this->command_buffers.size(); i++)
	{
		this->command_buffers[i] = new VulkanCommandBuffer(device, command_pool, pipeline_pool, descriptor_pool,  transfer_buffer, (uint32_t)i);
	}
}

VulkanCommandBufferSet::~VulkanCommandBufferSet()
{
	for (size_t i = 0; i < this->command_buffers.size(); i++)
	{
		delete this->command_buffers[i];
	}
}

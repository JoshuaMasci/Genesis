#include "VulkanCommandBuffer.hpp"

using namespace Genesis;

VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer command_buffer, VkRenderPass render_pass)
{
	this->command_buffer = command_buffer;
	this->render_pass = render_pass;
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
}

void VulkanCommandBuffer::setShader(ShaderHandle shader)
{
	this->current_shader = (VulkanShader*)shader;
}

void VulkanCommandBuffer::setPipelineSettings(PipelineSettings& settings)
{
	this->current_settings = settings;
}

void VulkanCommandBuffer::setScissor(vector2U offset, vector2U extent)
{
	VkRect2D rect = {};
	rect.offset = {offset.x, offset.y};
	rect.extent = {extent.x, extent.y};
	vkCmdSetScissor(this->command_buffer, 0, 1, &rect);
}

void VulkanCommandBuffer::setUniformFloat(string name, float value)
{
}

void VulkanCommandBuffer::setUniformVec4(string name, vector4F vector)
{
}

void VulkanCommandBuffer::setUniformMat4(string name, matrix4F matrix)
{
}

void VulkanCommandBuffer::setUniformTexture(string name, TextureHandle texture)
{
}

void VulkanCommandBuffer::setUniformView(string name, ViewHandle view, uint16_t view_image_index)
{

}

void VulkanCommandBuffer::drawIndexed(VertexBufferHandle vertex_handle, IndexBufferHandle index_handle)
{
}

void VulkanCommandBuffer::drawIndexedOffset(VertexBufferHandle vertex_handle, IndexBufferHandle index_handle, uint32_t index_offset, uint32_t index_count)
{
}

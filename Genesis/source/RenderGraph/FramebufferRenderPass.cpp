#include "Genesis/RenderGraph/FramebufferRenderPass.hpp"

#include "Genesis/Debug/Assert.hpp"

using namespace Genesis;

FramebufferRenderPass::FramebufferRenderPass(vector2U size)
	:framebuffer_size(size)
{
}

void FramebufferRenderPass::setBuildFunction(std::function<void(CommandBufferInterface*, vector2U)> function)
{
	this->build_function = function;
}

void FramebufferRenderPass::addColorOutput(ImageAttachmentCreateInfo attachment_info)
{
	ImageColorAttachment attachment = {};
	attachment.attachment_info = attachment_info;
	attachment.stages_read = 0;
	attachment.handle = nullptr;
	this->color_attachments.push_back(attachment);
}

void FramebufferRenderPass::addDepthStencilOutput(ImageAttachmentCreateInfo attachment_info)
{
	this->has_depth_stencil = true;

	this->depth_stencil_attachment.attachment_info = attachment_info;
	this->depth_stencil_attachment.stages_read = 0;
	this->depth_stencil_attachment.handle = nullptr;
}

FramebufferAttachement FramebufferRenderPass::getColorAttachement(uint32_t color_index)
{
	GENESIS_ENGINE_ASSERT_ERROR((this->is_compiled), "Renderpass has not been compiled");
	return this->color_attachments[color_index].handle;
}

FramebufferAttachement FramebufferRenderPass::getDepthStencilAttachement()
{
	GENESIS_ENGINE_ASSERT_ERROR((this->is_compiled), "Renderpass has not been compiled");
	return this->depth_stencil_attachment.handle;
}

void FramebufferRenderPass::build(CommandBufferInterface* command_buffer)
{
	this->build_function(command_buffer, this->framebuffer_size);
}
#include "Genesis/FrameGraph/RenderTask.hpp"

#include "Genesis/Debug/Assert.hpp"

using namespace Genesis;

RenderTask::RenderTask(RenderTaskSize size_type, vector2F size)
	:size_type(size_type), size(size)
{

}

void RenderTask::setBuildFunction(std::function<void(CommandBufferInterface*, vector2U)> function)
{
	this->build_function = function;
}


void RenderTask::addColorOutput(FramebufferAttachmentDescription color_description)
{
	FramebufferAttachment color_attachment = { this, 0, color_description.format, color_description.samples, nullptr };
	this->color_attachments.push_back(color_attachment);
}

void RenderTask::addDepthStencilOutput(FramebufferAttachmentDescription depth_description)
{
	this->has_depth_stencil = true;
	this->depth_stencil_attachment = { this, 0, depth_description.format, depth_description.samples, nullptr };
}

void RenderTask::build(CommandBufferInterface* command_buffer)
{
	this->build_function(command_buffer, (vector2U)this->size);
}
#include "Genesis/FrameGraph/RenderTask.hpp"

#include "Genesis/Debug/Assert.hpp"

#include "Genesis/FrameGraph/RenderTask.hpp"

using namespace Genesis;

size_t FrameTask::readFramebufferAttachment(FramebufferAttachment* attachment)
{
	size_t index = this->framebuffer_attachment_dependencies.size();
	this->framebuffer_attachment_dependencies.push_back(attachment);
	attachment->usage_counts++;
	return index;
}

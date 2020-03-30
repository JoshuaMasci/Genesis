#include "Genesis/RenderGraph/RenderGraph.hpp"

#include "Genesis/Debug/Assert.hpp"

using namespace Genesis;

void RenderPass::addColorInput(ShaderStage stages, FramebufferRenderPass* framebuffer, uint32_t color_index)
{
	this->color_input_attachments.push_back({ stages, framebuffer, color_index });
}

void RenderPass::addDepthStencilInput(ShaderStage stages, FramebufferRenderPass* framebuffer)
{
	this->depth_stencil_input_attachments.push_back({ stages, framebuffer });
}
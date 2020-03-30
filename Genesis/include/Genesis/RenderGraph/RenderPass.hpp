#pragma once

#include <functional>

#include "Genesis/Core/Types.hpp"
#include "Genesis/RenderingBackend/RenderingTypes.hpp"
#include "Genesis/RenderingBackend/CommandBuffer.hpp"

namespace Genesis
{
	class FramebufferRenderPass;

	class RenderPass
	{
	public:
		void addColorInput(ShaderStage stages, FramebufferRenderPass* framebuffer, uint32_t color_index);
		void addDepthStencilInput(ShaderStage stages, FramebufferRenderPass* framebuffer);

		virtual void build(CommandBufferInterface* command_buffer) = 0;

	protected:
		friend class RenderGraph;
		bool is_compiled = false;

		struct ColorInputDescripton
		{
			ShaderStage stages;
			FramebufferRenderPass* framebuffer;
			uint32_t color_index;
		};
		vector<ColorInputDescripton> color_input_attachments;

		struct DepthStencilInputDescripton
		{
			ShaderStage stages;
			FramebufferRenderPass* framebuffer;
		};
		vector<DepthStencilInputDescripton> depth_stencil_input_attachments;

	};

	//Compute Operations
	class ComputeRenderPass
	{

	};

	//Present to the Screen
	class SwapchainRenderPass
	{

	};
}
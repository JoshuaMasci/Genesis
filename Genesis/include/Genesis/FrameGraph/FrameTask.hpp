#pragma once

#include <functional>

#include "Genesis/Core/Types.hpp"
#include "Genesis/RenderingBackend/RenderingTypes.hpp"
#include "Genesis/RenderingBackend/CommandBuffer.hpp"

namespace Genesis
{
	struct FramebufferAttachment;

	class FrameTask
	{
	public:

		virtual void build(CommandBufferInterface* command_buffer) = 0;

		size_t readFramebufferAttachment(FramebufferAttachment* attachment);

	protected:
		friend class FrameGraph;
		bool is_compiled = false;

		vector<FramebufferAttachment*> framebuffer_attachment_dependencies;
	};

	//Compute Operations
	class ComputeTask
	{

	};
}
#pragma once

#include "Genesis/FrameGraph/FrameTask.hpp"

namespace Genesis
{
	class SwapchainRenderTask : public FrameTask
	{
	public:
		SwapchainRenderTask();

		void setBuildFunction(std::function<void(CommandBufferInterface*, vector2U)> function);
		virtual void build(CommandBufferInterface* command_buffer) override;

	protected:
		std::function<void(CommandBufferInterface*, vector2U)> build_function;
	};
}
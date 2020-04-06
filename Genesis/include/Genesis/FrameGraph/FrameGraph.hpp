#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/FrameGraph/FrameTask.hpp"
#include "Genesis/FrameGraph/RenderTask.hpp"
#include "Genesis/FrameGraph/SwapchainRenderTask.hpp"

namespace Genesis
{
	class FrameGraph
	{
	public:
		FrameGraph();
		~FrameGraph();

		RenderTask* createFramebufferRenderPass(RenderTaskSize size_type, vector2F size);
		SwapchainRenderTask* getSwapchainRenderPass();

		void compile();

		vector<RenderTask*>& getRenderTasks() { return this->render_passes; };

	protected:
		SwapchainRenderTask* root_task = nullptr;

		vector<RenderTask*> render_passes;

		vector<FrameTask*> ordered_render_tasks;
	};
}
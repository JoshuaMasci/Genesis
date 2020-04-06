#include "Genesis/FrameGraph/FrameGraph.hpp"

#include "Genesis/FrameGraph/RenderTask.hpp"
#include "Genesis/FrameGraph/SwapchainRenderTask.hpp"

#include "Genesis/Debug/Assert.hpp"

using namespace Genesis;

FrameGraph::FrameGraph()
{
}

FrameGraph::~FrameGraph()
{
	for (size_t i = 0; i < this->render_passes.size(); i++)
	{
		delete this->render_passes[i];
	}

	delete this->root_task;
}

RenderTask* FrameGraph::createFramebufferRenderPass(RenderTaskSize size_type, vector2F size)
{
	RenderTask* render_pass = new RenderTask(size_type, size);
	this->render_passes.push_back(render_pass);

	return render_pass;
}

SwapchainRenderTask* FrameGraph::getSwapchainRenderPass()
{
	if (this->root_task == nullptr)
	{
		this->root_task = new SwapchainRenderTask();
	}

	return this->root_task;
}

void FrameGraph::compile()
{
	std::queue<FrameTask*> render_pass_queue;
	render_pass_queue.push(this->root_task);

	while (!render_pass_queue.empty())
	{
		FrameTask* render_pass = render_pass_queue.front();
		render_pass_queue.pop();

		bool found = false;
		{
			for (size_t i = 0; i < this->ordered_render_tasks.size(); i++)
			{
				if (this->ordered_render_tasks[i] == render_pass)
				{
					found = true;
					break;
				}
			}
		}

		if (!found)
		{
			this->ordered_render_tasks.push_back(render_pass);

			for (size_t i = 0; i < render_pass->framebuffer_attachment_dependencies.size(); i++)
			{
				render_pass_queue.push((FrameTask*)render_pass->framebuffer_attachment_dependencies[i]->parent);
			}
		}
	}
}

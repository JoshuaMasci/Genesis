#include "Genesis/RenderGraph/RenderGraph.hpp"

#include "Genesis/Debug/Assert.hpp"

using namespace Genesis;

void RenderGraph::addRenderPass(RenderPass* render_pass)
{
	this->render_passes.push_back(render_pass);
}

void RenderGraph::compile()
{

}

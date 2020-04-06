#include "Genesis/FrameGraph/SwapchainRenderTask.hpp"

using namespace Genesis;

SwapchainRenderTask::SwapchainRenderTask()
{

}

void SwapchainRenderTask::setBuildFunction(std::function<void(CommandBufferInterface*, vector2U)> function)
{
	this->build_function = function;
}

void SwapchainRenderTask::build(CommandBufferInterface* command_buffer)
{
	this->build_function(command_buffer, vector2U());
}

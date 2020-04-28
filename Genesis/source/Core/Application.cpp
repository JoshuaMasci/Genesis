#include "Genesis/Core/Application.hpp"

#include "Genesis/Debug/Profiler.hpp"

using namespace Genesis;

Application::Application()
	:input_manager("config/input")
{

}

Application::~Application()
{
	//Needs to wait till the GPU finishes before deleting stuff
	if (this->rendering_backend != nullptr)
	{
		this->rendering_backend->waitTillDone();
	}

	if (this->rendering_backend != nullptr)
	{
		delete this->rendering_backend;
	}

	if (this->window != nullptr)
	{
		delete this->window;
	}

	if (this->platform != nullptr)
	{
		delete this->platform;
	}
}

void Application::run()
{
	//Fixed Timestep Stuff
	/*const double SimulationStep = 1.0 / (60.0 * 3);
	const TimeStep MaxTimeStep = 1.0 / 60.0;
	double accumulator = SimulationStep;*/


	//Mode: Rendering and Simulation Linked
	using clock = std::chrono::high_resolution_clock;
	auto time_last = clock::now();
	auto time_current = time_last;

	auto time_last_frame = time_last;
	size_t frames = 0;

	while (this->isRunning())
	{
		GENESIS_PROFILE_BLOCK_START("Application_Loop");

		time_current = clock::now();
		TimeStep time_step = (TimeStep)std::chrono::duration_cast<std::chrono::duration<double>>(time_current - time_last).count();
		/*if (time_step > MaxTimeStep)
		{
			time_step = MaxTimeStep;
		}

		accumulator += time_step;

		while (accumulator >= SimulationStep)
		{
			GENESIS_PROFILE_BLOCK_START("Application_SimulationStep");
			this->update(SimulationStep);
			accumulator -= SimulationStep;
			GENESIS_PROFILE_BLOCK_END();
		}

		TimeStep interpolation_value = accumulator / SimulationStep;*/

		this->update(time_step);

		this->render(time_step);

		time_last = time_current;

		frames++;
		double frame_delta = std::chrono::duration_cast<std::chrono::duration<double>>(time_last - time_last_frame).count();
		if (frame_delta > 1.0)
		{
			this->window->setWindowTitle("Sandbox: FPS " + std::to_string(frames));

			frames = 0;
			time_last_frame = time_last;
		}

		GENESIS_PROFILE_BLOCK_END();
	}

}

#include "Genesis/Entity/DebugCamera.hpp"
void Application::update(TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("Application::update");
	this->input_manager.update();

	if (this->platform != nullptr)
	{
		this->platform->onUpdate(time_step);
	}
}

#include "Genesis/FrameGraph/FrameGraph.hpp"

void Application::render(TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("Application::render");

	//RenderGraph test
	/*{
		FrameGraph* render_graph = new FrameGraph();

		//Shadow Task
		RenderTask* shadow_task = render_graph->createFramebufferRenderPass(RenderTaskSize::Absolute, vector2F(1000.0f));
		{
			FramebufferAttachmentDescription depth_info;
			depth_info.format = ImageFormat::D_16_Unorm;
			depth_info.samples = 1;
			shadow_task->addDepthStencilOutput(depth_info);
		}

		//Main Task
		RenderTask* main_task = render_graph->createFramebufferRenderPass(RenderTaskSize::SwapchainRelative, vector2F(1.0f));
		{
			FramebufferAttachmentDescription image_info;
			image_info.format = ImageFormat::RGBA_8_Unorm;
			image_info.samples = 1;
			main_task->addColorOutput(image_info);

			FramebufferAttachmentDescription depth_info;
			depth_info.format = ImageFormat::D_16_Unorm;
			depth_info.samples = 1;
			main_task->addDepthStencilOutput(depth_info);

			size_t shadow_index = main_task->readFramebufferAttachment(shadow_task->getDepthStencilAttachment());
		}

		//Ui Task
		RenderTask* ui_task = render_graph->createFramebufferRenderPass(RenderTaskSize::SwapchainRelative, vector2F(1.0f));
		{
			FramebufferAttachmentDescription image_info;
			image_info.format = ImageFormat::RGBA_8_Unorm;
			image_info.samples = 1;
			ui_task->addColorOutput(image_info);
		}

		//Swapchain
		SwapchainRenderTask* swapchain = render_graph->getSwapchainRenderPass();
		{
			size_t main_index = swapchain->readFramebufferAttachment(main_task->getColorAttachment(0));
			size_t ui_index = swapchain->readFramebufferAttachment(ui_task->getColorAttachment(0));
		}

		render_graph->compile();
		
		this->rendering_backend->submitFrameGraph(render_graph);

		delete render_graph;
	}*/
}

void Application::close()
{
	this->is_running = false;
}

bool Application::isRunning()
{
	return this->is_running;
}
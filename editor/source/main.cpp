#include "genesis_editor/genesis_editor.hpp"

#include "VulkanRenderer.hpp"
#include "FrameGraph.hpp"
#include "SDL2_Window.hpp" 
 
int main(int argc, char** argv)
{
	if(true)
	{
		{
			genesis_engine::SDL2_Window window = genesis_engine::SDL2_Window({ 1920, 1080 }, "Vulkan Window", false);

			VulkanRenderer::VulkanApplication app = {};
			app.app_name = "Genesis";
			app.app_version = 1;
			VulkanRenderer::VulkanSettings settings = {};

			VulkanRenderer::VulkanWindow window_info = {};
			window_info.handle = window.getNativeWindowHandle();
			window_info.width = window.getWindowSize().x;
			window_info.height = window.getWindowSize().y;

			VulkanRenderer::VulkanRenderer renderer = VulkanRenderer::VulkanRenderer(app, settings, window_info);

			VulkanRenderer::FrameGraph frame_graph;

			VulkanRenderer::RenderPassCreateInfo pass_info = {};
			VulkanRenderer::AttachmentCreateInfo color_info;
			color_info.format = VK_FORMAT_R8G8B8A8_SRGB;
			pass_info.color_attachments.push_back(color_info);
			VulkanRenderer::RenderPassDescription* render_pass = frame_graph.create_render_pass(pass_info);

			frame_graph.set_present_attachment(render_pass->color_attachments[0].id);

			for (size_t i = 0; i < 12000; i++)
			{
				renderer.render(&frame_graph);
			}
		}

		getchar();
		//return 0;
	}

	GENESIS_PROFILE_START(); 
	genesis_engine::Logging::inti_engine_logging();
	genesis_engine::Logging::inti_client_logging("Genesis_Editor");

	genesis_engine::EditorApplication* editor = new genesis_engine::EditorApplication();
	GENESIS_INFO("Genesis_Editor Started");

	GENESIS_PROFILE_BLOCK_START("Genesis_Editor Loop");
	editor->run();
	GENESIS_PROFILE_BLOCK_END();

	GENESIS_PROFILE_BLOCK_START("Genesis_Editor Exit");
	delete editor;
	GENESIS_PROFILE_BLOCK_END();

	GENESIS_INFO("Genesis_Editor Closed");

	GENESIS_PROFILE_WRITE_TO_FILE("Genesis_profile.prof");

	//Wait till enter
	//getchar();

	return 0;
}
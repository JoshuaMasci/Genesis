#include "genesis_editor/genesis_editor.hpp"

#include "renderer.hpp"
#include "SDL2_Window.hpp" 
#include "genesis_core/rendering/frame_graph.hpp"

int main(int argc, char** argv)
{
	if(true)
	{
		{
			genesis::SDL2_Window window = genesis::SDL2_Window({ 1920, 1080 }, "Vulkan Window", false);
			genesis::ApplicationInfo app = {};
			app.app_name = "Genesis";
			app.app_version = 1;
			genesis::Settings settings = {};
			const uint32_t max_count = 2048;
			settings.max_uniform_buffer_bindings = max_count;
			settings.max_storage_buffer_bindings = max_count;
			settings.max_sampled_image_bindings = max_count;
			settings.max_storage_image_bindings = max_count;
			
			genesis::WindowInfo window_info = {};
			window_info.handle = window.getNativeWindowHandle();
			window_info.width = window.getWindowSize().x;
			window_info.height = window.getWindowSize().y;

			genesis::Renderer renderer = genesis::Renderer(app, settings, window_info);

			genesis::FrameGraph frame_graph;
			genesis::RenderPassCreateInfo pass_info = {};
			genesis::AttachmentCreateInfo color_info;
			color_info.format = genesis::ImageFormat::RGBA_8_Srgb;
			pass_info.color_attachments.push_back(color_info);
			genesis::RenderPass* render_pass = frame_graph.create_render_pass(pass_info);

			frame_graph.set_present_attachment(render_pass->color_attachments[0].id);

			//for (size_t i = 0; i < 12000; i++)
			//{
			//	renderer.render(nullptr);
			//}
		}
	}

	GENESIS_PROFILE_START(); 
	genesis::Logging::inti_engine_logging();
	genesis::Logging::inti_client_logging("Genesis_Editor");

	genesis::EditorApplication* editor = new genesis::EditorApplication();
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
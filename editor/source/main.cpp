#include "genesis_editor/genesis_editor.hpp"

#include "vulkan_renderer/renderer.hpp"
#include "SDL2_Window.hpp" 
#include "genesis_core/render_graph/render_graph.hpp"
#include "genesis_engine/platform/file_system.hpp"

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
			settings.max_storage_buffer_bindings = max_count;
			settings.max_sampled_image_bindings = max_count;
			settings.max_storage_image_bindings = max_count;
			
			genesis::WindowInfo window_info = {};
			window_info.handle = window.getNativeWindowHandle();
			window_info.width = window.getWindowSize().x;
			window_info.height = window.getWindowSize().y;

			genesis::Renderer renderer = genesis::Renderer(app, settings, window_info);

			{
				genesis::ResourceId color_attachment = 0;

				genesis::vector<genesis::RenderTask> render_tasks;


			}

			for (size_t i = 0; i < 1000; i++)
			{
				renderer.draw_frame();
			}

		}
	}

	return 0;

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
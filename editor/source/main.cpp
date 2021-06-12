#include "genesis_editor/genesis_editor.hpp"

#include "vulkan_renderer/renderer.hpp"
#include "SDL2_Window.hpp" 
#include "genesis_core/rendering/frame_graph.hpp"
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

			genesis::ShaderModule vertex_shader = nullptr;
			genesis::vector<uint8_t> shader_data;
			genesis::FileSystem::loadFileBinary("res/vulkan/test.vert.spv", shader_data);
			vertex_shader = renderer.create_shader_module(shader_data.data(), shader_data.size());

			genesis::VertexHandle vertex_buffer = renderer.create_vertex_buffer(10000, genesis::MemoryType::GpuOnly);
			genesis::IndexHandle index_buffer = renderer.create_index_buffer(10000, genesis::MemoryType::GpuOnly);

			genesis::FrameGraph frame_graph;
			genesis::RenderPassCreateInfo pass_info = {};
			genesis::ColorAttachmentInfo color_info;
			color_info.format = genesis::ColorFormat::RGBA_8_Srgb;
			pass_info.color_attachments.push_back(color_info);
			genesis::RenderPass* render_pass = frame_graph.create_render_pass(pass_info);

			{
				genesis::PipelineCreateInfo create_info = {};
				create_info.vertex_elements = { genesis::VertexElementType::float_3 };
				create_info.vertex_shader = vertex_shader;
				render_pass->create_pipelines.push_back(create_info);
			};

			frame_graph.set_present_attachment(render_pass->color_attachments[0].id);

			for (size_t i = 0; i < 100; i++)
			{
				renderer.render(&frame_graph);
			}

			renderer.destroy_shader_module(vertex_shader);
			renderer.destroy_vertex_buffer(vertex_buffer);
			renderer.destroy_index_buffer(index_buffer);
		}
	}

	return;

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
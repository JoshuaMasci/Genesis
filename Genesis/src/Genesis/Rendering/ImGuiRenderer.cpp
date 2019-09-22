#include "ImGuiRenderer.hpp" 

using namespace Genesis;

#include "imgui.cpp"
#include "imgui.h"
#include "imgui_demo.cpp"
#include "imgui_draw.cpp"
#include "imgui_widgets.cpp"
#include "imgui_internal.h"

#include <fstream>

ImGuiRenderer::ImGuiRenderer(RenderingBackend* backend, InputManager* input_manager)
{
	this->backend = backend;
	this->input_manager = input_manager;

	vector2U window_size = backend->getScreenSize();

	Array<ImageFormat> color(1);
	color[0] = ImageFormat::RGBA_8_UNorm;
	this->layout = FramebufferLayout(color, ImageFormat::Invalid);

	this->view = this->backend->createView(window_size, this->layout);

	ImGui::CreateContext();

	{
		ImGuiIO& io = ImGui::GetIO();
		int width, height;
		unsigned char* pixels = NULL;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		this->texture_atlas = this->backend->createTexture(vector2U(width, height), pixels, (width * height) * 4);
		io.Fonts->TexID = this->texture_atlas;

		io.DisplaySize.x = (float)window_size.x;
		io.DisplaySize.y = (float)window_size.y;
	}

	{
		string vert_file_path = "resources/imgui/vulkan.vert.spv";
		string frag_file_path = "resources/imgui/vulkan.frag.spv";

		string vert_data = "";
		string frag_data = "";

		std::ifstream vert_file(vert_file_path, std::ios::ate | std::ios::binary);
		if (vert_file.is_open())
		{
			size_t fileSize = (size_t)vert_file.tellg();
			vert_file.seekg(0);
			vert_data.resize(fileSize);
			vert_file.read(vert_data.data(), vert_data.size());
			vert_file.close();
		}

		std::ifstream frag_file(frag_file_path, std::ios::ate | std::ios::binary);
		if (frag_file.is_open())
		{
			size_t fileSize = (size_t)frag_file.tellg();
			frag_file.seekg(0);
			frag_data.resize(fileSize);
			frag_file.read(frag_data.data(), frag_data.size());
			frag_file.close();
		}

		this->shader = this->backend->createShader(vert_data, frag_data);
	}


	this->vertex_input = VertexInputDescription
	({
		{"in_position", VertexElementType::float_2},
		{"in_uv", VertexElementType::float_2},
		{"in_color", VertexElementType::unorm8_4}
	});

	this->settings.cull_mode = CullMode::None;
	this->settings.depth_test = DepthTest::None;
	this->settings.depth_op = DepthOp::Always;
	this->settings.blend_op = BlendOp::Add;
	this->settings.src_factor = BlendFactor::Alpha_Src;
	this->settings.dst_factor = BlendFactor::One_Minus_Alpha_Src;
}

ImGuiRenderer::~ImGuiRenderer()
{
	this->backend->destroyView(this->view);
	this->backend->destroyTexture(this->texture_atlas);
	this->backend->destroyShader(this->shader);
}

void ImGuiRenderer::startFrame()
{
	ImGuiIO& io = ImGui::GetIO();

	//TODO Input
	vector2F mouse_pos = this->input_manager->getMousePosition();
	io.MousePos = ImVec2(mouse_pos.x, mouse_pos.y);

	io.MouseDown[0] = this->input_manager->getButtonDown("Mouse_Left");
	io.MouseDown[1] = this->input_manager->getButtonDown("Mouse_Right");
	io.MouseDown[2] = this->input_manager->getButtonDown("Mouse_Middle");
	io.MouseDown[3] = this->input_manager->getButtonDown("Mouse_X1");
	io.MouseDown[4] = this->input_manager->getButtonDown("Mouse_X2");

	if (this->input_manager->getButtonDown("Mouse_ForwardScroll"))
	{
		io.MouseWheel += 1;
	}
	else if (this->input_manager->getButtonDown("Mouse_BackwardScroll"))
	{
		io.MouseWheel -= 1;
	}

	ImGui::NewFrame();

	bool show_demo = true;
	ImGui::ShowDemoWindow(&show_demo);
}

void ImGuiRenderer::endFrame()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImDrawData* draw_data = ImGui::GetDrawData();

	//this->backend->startView(this->view);
	CommandBuffer* command_buffer = this->backend->getScreenCommandBuffer();

	command_buffer->setPipelineSettings(this->settings);

	command_buffer->setShader(this->shader);
	vector2F scale;
	scale[0] = 2.0f / draw_data->DisplaySize.x;
	scale[1] = 2.0f / draw_data->DisplaySize.y;
	vector2F translate;
	translate[0] = -1.0f - (draw_data->DisplayPos.x * scale[0]);
	translate[1] = -1.0f - (draw_data->DisplayPos.y * scale[1]);

	command_buffer->setUniformVec2("offset.uScale", scale);
	command_buffer->setUniformVec2("offset.uTranslate", translate);

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;

		VertexBufferHandle vertex_buffer = this->backend->createVertexBuffer(cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.size_in_bytes(), this->vertex_input, MemoryUsage::CPU_Visable);
		IndexBufferHandle index_buffer = this->backend->createIndexBuffer(cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.size(), IndexType::uint16);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				//TODO Scissor
				//command_buffer->setScissor
				ImVec2 pos = draw_data->DisplayPos;
				command_buffer->setScissor
				(
					{ (uint32_t)(pcmd->ClipRect.x - pos.x), (uint32_t)(pcmd->ClipRect.y - pos.y) },
					{ (uint32_t)(pcmd->ClipRect.z - pos.x), (uint32_t)(pcmd->ClipRect.w - pos.y) }
				);

				command_buffer->setUniformTexture("texture_atlas", (TextureHandle)pcmd->TextureId);
				command_buffer->drawIndexedOffset(vertex_buffer, index_buffer, pcmd->IdxOffset, pcmd->ElemCount);
			}
		}
		this->backend->destroyVertexBuffer(vertex_buffer);
		this->backend->destroyIndexBuffer(index_buffer);
	}

	//this->backend->endView(this->view);
	//this->backend->sumbitView(this->view);
}

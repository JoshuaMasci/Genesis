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
	:RenderLayer(backend)
{
	this->input_manager = input_manager;

	this->view_size = backend->getScreenSize();


	Array<ImageFormat> color(1);
	color[0] = ImageFormat::RGBA_8_Unorm;
	this->layout = FramebufferLayout(color, ImageFormat::Invalid);

	this->view = this->backend->createView(this->view_size, this->layout, CommandBufferType::SingleThread);

	ImGui::CreateContext();

	{
		ImGuiIO& io = ImGui::GetIO();
		int width, height;
		unsigned char* pixels = NULL;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		this->texture_atlas = this->backend->createTexture(vector2U(width, height), pixels, (width * height) * 4);
		io.Fonts->TexID = this->texture_atlas;

		io.DisplaySize.x = (float)this->view_size.x;
		io.DisplaySize.y = (float)this->view_size.y;

		io.IniFilename = NULL;
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
	vector2U temp_size = this->backend->getScreenSize();
	if (temp_size != this->view_size)
	{
		this->view_size = temp_size;
		this->backend->resizeView(this->view, this->view_size);
	}

	ImGuiIO& io = ImGui::GetIO();

	io.DisplaySize = { (float)this->view_size.x, (float)this->view_size.y };
	
	vector2F mouse_pos = this->input_manager->getMousePosition();
	io.MousePos = ImVec2(mouse_pos.x, mouse_pos.y);

	//TODO Input

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
}

void ImGuiRenderer::endFrame()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImDrawData* draw_data = ImGui::GetDrawData();

	this->backend->startView(this->view);
	CommandBuffer* command_buffer = this->backend->getViewCommandBuffer(this->view);
	if (command_buffer == nullptr)
	{
		return;
	}

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

	ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
	ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;

		VertexBuffer vertex_buffer = this->backend->createVertexBuffer(cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.size_in_bytes(), this->vertex_input, MemoryUsage::CPU_Visable);
		IndexBuffer index_buffer = this->backend->createIndexBuffer(cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.size(), IndexType::uint16);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				// Project scissor/clipping rectangles into framebuffer space
				ImVec4 clip_rect;
				clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
				clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
				clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
				clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

				if (clip_rect.x < draw_data->DisplaySize.x && clip_rect.y < draw_data->DisplaySize.y && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
				{
					// Negative offsets are illegal for Set Scissor
					if (clip_rect.x < 0.0f)
						clip_rect.x = 0.0f;
					if (clip_rect.y < 0.0f)
						clip_rect.y = 0.0f;

					// Apply scissor/clipping rectangle
					vector2I offset = { clip_rect.x, clip_rect.y };
					vector2U extend = { (uint32_t)(clip_rect.z - clip_rect.x),  (uint32_t)(clip_rect.w - clip_rect.y) };
					command_buffer->setScissor(offset, extend);

					command_buffer->setUniformTexture("texture_atlas", (Texture)pcmd->TextureId);
					command_buffer->drawIndexedOffset(vertex_buffer, index_buffer, pcmd->IdxOffset, pcmd->ElemCount);
				}
			}
		}
		this->backend->destroyVertexBuffer(vertex_buffer);
		this->backend->destroyIndexBuffer(index_buffer);
	}

	this->backend->endView(this->view);
	this->backend->submitView(this->view);
}

void ImGuiRenderer::setScreenSize(vector2U size)
{
	this->backend->resizeView(this->view, size);
}

View ImGuiRenderer::getView()
{
	return this->view;
}

uint32_t ImGuiRenderer::getViewImageIndex()
{
	return 0;//First Framebuffer Image
}

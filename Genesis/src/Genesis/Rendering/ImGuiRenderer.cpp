#include "ImGuiRenderer.hpp" 

using namespace Genesis;

#include "imgui.cpp"
#include "imgui.h"
#include "imgui_demo.cpp"
#include "imgui_draw.cpp"
#include "imgui_widgets.cpp"
#include "imgui_internal.h"

#include <fstream>

ImGuiRenderer::ImGuiRenderer(RenderingBackend* backend)
{
	this->backend = backend;
	
	Array<ImageFormat> color(1);
	color[0] = ImageFormat::RGBA_8_UNorm;
	this->layout = FramebufferLayout(color, ImageFormat::Invalid);

	this->view = this->backend->createView(vector2U(1920, 1080), this->layout);

	ImGui::CreateContext();

	{
		ImGuiIO& io = ImGui::GetIO();
		int width, height;
		unsigned char* pixels = NULL;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		this->texture_atlas = this->backend->createTexture(vector2U(width, height), pixels, (width * height) * 4);
		io.Fonts->TexID = this->texture_atlas;
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
	io.DisplaySize.x = 1920.0f;
	io.DisplaySize.y = 1080.0f;

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
				//this->backend->tempDrawView(this->view, vertex_buffer, index_buffer, this->shader, (TextureHandle) pcmd->TextureId, pcmd->IdxOffset, pcmd->ElemCount, vector2I(0.0), vector2U(0.0));
			}
		}
		this->backend->destroyVertexBuffer(vertex_buffer);
		this->backend->destroyIndexBuffer(index_buffer);
	}

	//this->backend->endView(this->view);
	//this->backend->sumbitView(this->view);
}

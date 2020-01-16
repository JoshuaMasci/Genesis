#include "ImGuiRenderer.hpp"

using namespace Genesis;

#include "Genesis/Rendering/ResourceLoaders.hpp"

#include "imgui.cpp"
#include "imgui.h"
#include "imgui_demo.cpp"
#include "imgui_draw.cpp"
#include "imgui_widgets.cpp"
#include "imgui_internal.h"

Genesis::ImGuiRenderer::ImGuiRenderer(RenderingBackend* backend, InputManager* input_manager)
	:RenderLayer(backend)
{
	this->backend = backend;
	this->input_manager = input_manager;

	this->view_size = backend->getScreenSize();

	List<ImageFormat> color(1);
	color[0] = ImageFormat::RGBA_8_Unorm;
	this->layout = FramebufferLayout(color, ImageFormat::Invalid);
	this->view = this->backend->createView(this->layout, this->view_size);

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

		//io.IniFilename = NULL;
	}

	this->shader = ShaderLoader::loadShaderSingle(this->backend, "res/imgui/vulkan");

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

	this->sampler.min_filter = FilterMode::Nearest;
	this->sampler.mag_filter = FilterMode::Nearest;
	this->sampler.mipmap_mode = FilterMode::Nearest;
	this->sampler.U_address_mode = AddressMode::Repeat;
	this->sampler.V_address_mode = AddressMode::Repeat;
	this->sampler.W_address_mode = AddressMode::Repeat;
	this->sampler.mip_lod_bias = 0.0f;
	this->sampler.max_anisotropy = 0;
	this->sampler.compare_op = CompareOp::Never;
	this->sampler.min_lod = 0.0f;
	this->sampler.max_lod = 0.0f;
	this->sampler.border_color = BorderColor::Transparent_Black;
}

Genesis::ImGuiRenderer::~ImGuiRenderer()
{
	this->backend->destroyTexture(this->texture_atlas);
	this->backend->destroyShader(this->shader);
	this->backend->destroyView(this->view);
}

void Genesis::ImGuiRenderer::startLayer()
{
	vector2U temp_size = this->backend->getScreenSize();
	if (temp_size != this->view_size)
	{
		this->view_size = temp_size;
		this->backend->resizeView(this->view, this->view_size);
	}

	this->view_size = this->backend->getScreenSize();

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

void Genesis::ImGuiRenderer::endLayer()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImDrawData* draw_data = ImGui::GetDrawData();

	CommandBuffer* command_buffer = this->backend->beginView(this->view);
	command_buffer->setPipelineSettings(this->settings);
	command_buffer->setShader(this->shader);

	vector2F scale;

	vector4F push_constant;
	//scale x, y
	push_constant[0] = 2.0f / draw_data->DisplaySize.x;
	push_constant[1] = 2.0f / draw_data->DisplaySize.y;
	//translate x, y;
	push_constant[2] = -1.0f - (draw_data->DisplayPos.x * scale[0]);
	push_constant[3] = -1.0f - (draw_data->DisplayPos.y * scale[1]);

	command_buffer->setUniformConstant(&push_constant, sizeof(push_constant));

	ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
	ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;

		VertexBuffer vertex_buffer = this->backend->createVertexBuffer(cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.size_in_bytes(), this->vertex_input, MemoryUsage::CPU_Visable);
		IndexBuffer index_buffer = this->backend->createIndexBuffer(cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.size_in_bytes(), IndexType::uint16, MemoryUsage::CPU_Visable);

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

					command_buffer->setUniformTexture(0, 0, (Texture)pcmd->TextureId, this->sampler);

					command_buffer->setVertexBuffer(vertex_buffer, this->vertex_input);
					command_buffer->setIndexBuffer(index_buffer, IndexType::uint16);

					command_buffer->drawIndexed(pcmd->ElemCount, pcmd->IdxOffset);
				}
			}
		}
		this->backend->destroyVertexBuffer(vertex_buffer);
		this->backend->destroyIndexBuffer(index_buffer);
	}

	this->backend->endView(this->view);
}


#include "genesis_engine/LegacyRendering/LegacyImGui.hpp"

#include "imgui.h"

namespace Genesis
{
	const string vert_file = "#version 450\nlayout(location = 0) in vec2 in_position;\nlayout(location = 1) in vec2 in_uv;\nlayout(location = 2) in vec4 in_color;\nlayout(location = 0) out vec2 uv;\nlayout(location = 1) out vec4 color;\nstruct Offset\n{ \n	vec2 uScale; \n	vec2 uTranslate;\n};\nuniform Offset offset;\nvoid main()\n{\n	gl_Position = vec4(in_position  * offset.uScale + offset.uTranslate, 0.0, 1.0);\n	uv = in_uv;\n	color = in_color / 256.0;\n}";
	const string frag_file = "#version 450\nuniform sampler2D texture_atlas;\nlayout(location = 0) in vec2 uv;\nlayout(location = 1) in vec4 color;\nlayout(location = 0) out vec4 out_color;\nvoid main()\n{\n out_color = color * texture2D(texture_atlas, uv);\n}";

	LegacyImGui::LegacyImGui(LegacyBackend* backend, InputManager* input_manager, Window* window)
		:BaseImGui(input_manager, window)
	{
		this->backend = backend;

		ImGuiIO& io = ImGui::GetIO();

		int width, height, bytes_per_pixel;
		unsigned char* pixels = NULL;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

		TextureCreateInfo create_info = {};
		create_info.size = vector2U(width, height);
		create_info.format = ImageFormat::RGBA_8_Unorm;
		create_info.filter_mode = TextureFilterMode::Nearest;
		create_info.wrap_mode = TextureWrapMode::Clamp_Edge;
		this->texture_atlas = this->backend->createTexture(create_info, pixels);
		io.Fonts->TexID = (ImTextureID)this->texture_atlas;

		//Shader
		this->imgui_program = this->backend->createShaderProgram(vert_file.data(), (uint32_t)vert_file.size(), frag_file.data(), (uint32_t)frag_file.size());

		//VertexDescription
		this->vertex_elements = { VertexElementType::float_2, VertexElementType::float_2, VertexElementType::unorm8_4 };
		this->vertex_create_info.input_elements = this->vertex_elements.data();
		this->vertex_create_info.input_elements_count = (uint32_t)this->vertex_elements.size();

		//PipelineSettings
		this->settings.cull_mode = CullMode::None;
		this->settings.depth_test = DepthTest::None;
		this->settings.depth_op = DepthOp::Always;
		this->settings.blend_op = BlendOp::Add;
		this->settings.src_factor = BlendFactor::Alpha_Src;
		this->settings.dst_factor = BlendFactor::One_Minus_Alpha_Src;


		struct TempVertex
		{
			vector2F pos;
			vector2F uv;
			uint8_t color[4];
		};

		TempVertex vertex[] =
		{
			{vector2F(0.0f, 0.0f), vector2F(0.0f, 0.0f), {255, 0, 255, 255}},
			{vector2F(0.0f, 100.0f), vector2F(0.0f, 1.0f), {255, 0, 255, 255}},
			{vector2F(100.0f, 100.0f), vector2F(1.0f, 1.0f), {255, 0, 255, 255}},
			{vector2F(100.0f, 0.0f), vector2F(1.0f, 0.0f), {255, 0, 255, 255}}
		};

		uint16_t index[] = { 0, 1, 2, 2, 3, 0 };

		this->temp_vertex = this->backend->createVertexBuffer(vertex, sizeof(TempVertex) * _countof(vertex), this->vertex_create_info);
		this->temp_index = this->backend->createIndexBuffer(index, sizeof(uint16_t) * _countof(index), IndexType::uint16);
	}

	LegacyImGui::~LegacyImGui()
	{
		this->backend->destoryTexture(this->texture_atlas);
		this->backend->destoryShaderProgram(this->imgui_program);
	}

	void LegacyImGui::beginFrame()
	{
		this->update();
		ImGui::NewFrame();
	}

	void LegacyImGui::endFrame()
	{
		ImGui::EndFrame();
		ImGui::Render();
		ImDrawData* draw_data = ImGui::GetDrawData();

		int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
		int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
		if (fb_width <= 0 || fb_height <= 0)
			return;

		this->backend->bindShaderProgram(this->imgui_program);
		this->backend->setPipelineState(this->settings);

		vector2F scale;
		vector2F translate;
		scale.x = 2.0f / draw_data->DisplaySize.x;
		scale.y = 2.0f / draw_data->DisplaySize.y;
		translate.x = -1.0f - (draw_data->DisplayPos.x * scale.x);
		translate.y = -1.0f - (draw_data->DisplayPos.y * scale.y);
		this->backend->setUniform2f("offset.uScale", scale);
		this->backend->setUniform2f("offset.uTranslate", translate);

		ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
		ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;

			VertexBuffer vertex_buffer = this->backend->createVertexBuffer(cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.size_in_bytes(), vertex_create_info);
			IndexBuffer index_buffer = this->backend->createIndexBuffer(cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.size_in_bytes(), IndexType::uint16);
			this->backend->bindVertexBuffer(vertex_buffer);
			this->backend->bindIndexBuffer(index_buffer);

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
					if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
					{
						// Apply scissor/clipping rectangle
						vector2I offset = { (int)clip_rect.x, (int)(fb_height - clip_rect.w) };
						vector2U extend = { (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y) };
						this->backend->setScissor(offset, extend);
						this->backend->setUniformTexture("texture_atlas", 0, (Texture2D)pcmd->TextureId);
						this->backend->drawIndex(pcmd->ElemCount, pcmd->IdxOffset);
					}
				}
			}

			this->backend->bindVertexBuffer(nullptr);
			this->backend->bindIndexBuffer(nullptr);
			this->backend->destoryVertexBuffer(vertex_buffer);
			this->backend->destoryIndexBuffer(index_buffer);
		}
		this->backend->clearScissor();

		/*PipelineSettings settings;
		settings.cull_mode = CullMode::None;
		settings.depth_test = DepthTest::None;
		this->backend->setPipelineState(settings);

		this->backend->bindVertexBuffer(this->temp_vertex);
		this->backend->bindIndexBuffer(this->temp_index);

		vector2I offset = { draw_data->DisplaySize.x - 20, draw_data->DisplaySize.y - 20 };
		vector2U extend = { 20, 20 };
		this->backend->setScissor(offset, extend);

		this->backend->drawIndex(6, 0);

		this->backend->clearScissor();*/
	}
}
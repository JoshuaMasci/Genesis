#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/RenderingBackend/RenderingBackend.hpp"
#include "Genesis/Input/InputManager.hpp"
#include "Genesis/Rendering/RenderLayer.hpp"

namespace Genesis
{
	class ImGuiRenderer : public RenderLayer
	{
	public:
		ImGuiRenderer(RenderingBackend* backend, InputManager* input_manager);
		~ImGuiRenderer();

		void startLayer();
		void endLayer();

		virtual void ImGuiDraw() override;

		virtual inline Framebuffer getFramebuffer() { return this->framebuffer; };
		virtual inline uint32_t getFramebufferIndex() { return 0; };

	private:
		InputManager* input_manager;

		vector2U view_size;
		FramebufferLayout layout;
		Framebuffer framebuffer = nullptr;
		STCommandBuffer st_command_buffer;
		
		Texture texture_atlas = nullptr;
		Shader shader = nullptr;

		VertexInputDescription vertex_input;
		PipelineSettings settings;

		Sampler sampler;
	};
}
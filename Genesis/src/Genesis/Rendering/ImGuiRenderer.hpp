#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/RenderingBackend.hpp"
#include "Genesis/Input/InputManager.hpp"

namespace Genesis
{
	class ImGuiRenderer
	{
	public:
		ImGuiRenderer(RenderingBackend* backend, InputManager* input_manager);
		~ImGuiRenderer();

		void startFrame();
		void drawFrame(CommandBuffer* command_buffer);

	private:
		RenderingBackend* backend;
		InputManager* input_manager;

		//FramebufferLayout layout;
		//View view = nullptr;
		vector2U view_size;
		
		Texture texture_atlas = nullptr;
		Shader shader = nullptr;

		VertexInputDescription vertex_input;
		PipelineSettings settings;

		Sampler sampler;
	};
}
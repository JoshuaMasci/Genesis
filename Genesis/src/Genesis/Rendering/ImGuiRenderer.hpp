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
		void endFrame();

	private:
		RenderingBackend* backend;
		InputManager* input_manager;

		FramebufferLayout layout;

		ViewHandle view = nullptr;
		TextureHandle texture_atlas = nullptr;
		ShaderHandle shader = nullptr;

		VertexInputDescription vertex_input;
		PipelineSettings settings;
	};
}
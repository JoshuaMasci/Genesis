#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/RenderingBackend/RenderingBackend.hpp"
#include "Genesis/Input/InputManager.hpp"

namespace Genesis
{
	class ImGuiRenderer
	{
	public:
		ImGuiRenderer(RenderingBackend* backend, InputManager* input_manager);
		~ImGuiRenderer();

		void startLayer();
		void endLayer();

	private:
		InputManager* input_manager;
		
		Texture texture_atlas = nullptr;

		VertexInputDescription vertex_input;
		PipelineSettings settings;

		Sampler sampler;
	};
}
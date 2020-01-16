#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/RenderingBackend.hpp"
#include "Genesis/Input/InputManager.hpp"
#include "Genesis/Rendering/RenderLayer.hpp"

namespace Genesis
{
	class ImGuiRenderer : public RenderLayer
	{
	public:
		ImGuiRenderer(RenderingBackend* backend, InputManager* input_manager);
		~ImGuiRenderer();

		virtual void startLayer();
		virtual void endLayer();

		virtual inline View getView() { return this->view; };
		virtual inline uint32_t getViewImageIndex() { return 0; };

	private:
		InputManager* input_manager;

		vector2U view_size;
		FramebufferLayout layout;
		View view = nullptr;
		
		Texture texture_atlas = nullptr;
		Shader shader = nullptr;

		VertexInputDescription vertex_input;
		PipelineSettings settings;

		Sampler sampler;
	};
}
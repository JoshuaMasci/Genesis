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

		virtual void startFrame() override;
		virtual void endFrame() override;

		virtual void setScreenSize(vector2U size) override;

		virtual View getView() override;
		virtual uint32_t getViewImageIndex() override;

	private:
		InputManager* input_manager;

		FramebufferLayout layout;

		View view = nullptr;
		vector2U view_size;
		
		Texture texture_atlas = nullptr;
		Shader shader = nullptr;

		VertexInputDescription vertex_input;
		PipelineSettings settings;
	};
}
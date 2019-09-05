#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/RenderingBackend.hpp"

namespace Genesis
{
	class ImGuiRenderer
	{
	public:
		ImGuiRenderer(RenderingBackend* backend);
		~ImGuiRenderer();

		void startFrame();
		void endFrame();

	private:
		//Lifetime of the Backend is longer than the Renderer
		//No deleting please
		RenderingBackend* backend;

		FramebufferLayout layout;

		ViewHandle view = nullptr;
		TextureHandle texture_atlas = nullptr;
		ShaderHandle shader = nullptr;


		VertexInputDescription vertex_input;
	};
}
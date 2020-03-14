#pragma once

#include "Genesis/Rendering/RenderingBackend.hpp"
#include "Genesis/Rendering/RenderLayer.hpp"

namespace Genesis
{
	class RenderSystem
	{
	public:
		RenderSystem(RenderingBackend* backend);
		~RenderSystem();

		bool startFrame();
		void endFrame();

		void drawLayerWholeScreen(RenderLayer* layer);

	protected:
		RenderingBackend* backend = nullptr;

		CommandBufferInterface* command_buffer = nullptr;

		Shader screen_shader = nullptr;
		StaticBuffer screen_vertex;
		StaticBuffer screen_index;
		Sampler screen_sampler;
		PipelineSettings screen_settings;

		VertexInputDescription vertex_description;
	};
}
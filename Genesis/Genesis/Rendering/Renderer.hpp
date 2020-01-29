#pragma once

#include "Genesis/Rendering/RenderLayer.hpp"
#include "Genesis/Rendering/ResourceLoaders.hpp"

namespace Genesis
{
	class Renderer : public RenderLayer
	{
	public:
		Renderer(RenderingBackend* backend);
		~Renderer();

		virtual void startLayer() override;
		virtual void endLayer() override;

		virtual inline Framebuffer getFramebuffer() { return this->framebuffer; };
		virtual inline uint32_t getFramebufferIndex() { return 0; };

		void drawModel(Shader shader, PipelineSettings& settings, MeshTemp& mesh, void* uniform_const = nullptr, uint32_t uniform_const_size = 0);

		float scale = 1.0f;

	protected:
		vector2U view_size;
		FramebufferLayout layout;

		Framebuffer framebuffer;
		MTCommandBuffer mt_command_buffer;

		CommandBuffer* command_buffer = nullptr;

		//TEMP
		Shader shader;
		MeshTemp mesh;

		struct ColorUniform
		{
			vector4F color;
			matrix4F view_projection;
			matrix4F model;
		};
	};
}
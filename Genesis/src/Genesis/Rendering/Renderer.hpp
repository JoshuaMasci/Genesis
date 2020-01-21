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

		virtual View getView() override;
		virtual uint32_t getViewImageIndex() override;

		void drawModel(Shader shader, PipelineSettings& settings, Mesh& mesh, void* uniform_const = nullptr, uint32_t uniform_const_size = 0);

		float scale = 1.0f;

	protected:
		vector2U view_size;
		FramebufferLayout layout;
		View view = nullptr;

		Framebuffer framebuffer;
		MTCommandBuffer mt_command_buffer;

		CommandBuffer* command_buffer = nullptr;

		//TEMP
		Shader shader;
		Mesh mesh;

		struct ColorUniform
		{
			vector4F color;
			matrix4F view_projection;
			matrix4F model;
		};
	};
}
#pragma once

#include "Genesis/Rendering/RenderLayer.hpp"
#include "Genesis/Scene/Scene.hpp"

namespace Genesis
{
	class SceneRenderer : public RenderLayer
	{
	public:
		SceneRenderer(RenderingBackend* backend);
		~SceneRenderer();

		virtual void startLayer() override;
		virtual void endLayer() override;

		virtual inline Framebuffer getFramebuffer() { return this->framebuffer; };
		virtual inline uint32_t getFramebufferIndex() { return 0; };

		void drawScene(Scene* scene);

	protected:
		vector2U view_size;
		FramebufferLayout layout;

		Framebuffer framebuffer;
		MTCommandBuffer mt_command_buffer;

		CommandBuffer* command_buffer = nullptr;

		//Scene: Binding Set 0
		struct SceneUniform
		{
			alignas(16) vector3F camera_position;
			alignas(16) vector3F ambient_light;
			alignas(16) matrix4F view_projection_matrix;
		};
		SceneUniform scene_uniform;
		DynamicBuffer scene_uniform_buffer;

		//Material: Binding Set 1
		//Textures

		//Object Transform: Push Constant
		struct ObjectTransformUniform
		{
			matrix4F model_matrix;
		};
	};
}
#pragma once

#include "Genesis/Rendering/RenderLayer.hpp"
#include "Genesis/Resource/Material.hpp"
#include "Genesis/Entity/World.hpp"

namespace Genesis
{
	class WorldRenderer : public RenderLayer
	{
	public:
		WorldRenderer(RenderingBackend* backend);
		~WorldRenderer();

		virtual void ImGuiDraw() override;

		virtual inline Framebuffer getFramebuffer() { return this->framebuffer; };
		virtual inline uint32_t getFramebufferIndex() { return 0; };

		void drawWorld(World* world);

		//Framebuffer drawWorld(World* world, EntityHandle camera_entity, );

	protected:
		//TEMP
		Shader mesh_shader = nullptr;
		//END TEMP

		uint32_t draw_call_count = 0;
		bool use_frustum_culling = true;

		vector2U view_size;
		FramebufferLayout layout;

		Framebuffer framebuffer;
		STCommandBuffer st_command_buffer;

		//Scene: Binding Set 0
		struct SceneUniform
		{
			alignas(16) vector3F camera_position;
			alignas(16) vector3F ambient_light;
			alignas(16) matrix4F view_projection_matrix;
		};
		SceneUniform scene_uniform;
		DynamicBuffer scene_uniform_buffer;


		//Object Transform: Push Constant
		struct ObjectTransformUniform
		{
			matrix4F model_matrix;
			glm::mat3x4 normal_matrix;
		};
	};
}
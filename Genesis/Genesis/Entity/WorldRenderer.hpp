#pragma once

#include "Genesis/Rendering/RenderLayer.hpp"
#include "Genesis/Scene/Material.hpp"
#include "Genesis/Entity/World.hpp"

namespace Genesis
{
	class WorldRenderer : public RenderLayer
	{
	public:
		WorldRenderer(RenderingBackend* backend);
		~WorldRenderer();

		virtual void startLayer() override;
		virtual void endLayer() override;

		virtual void ImGuiDraw() override;

		virtual inline Framebuffer getFramebuffer() { return this->framebuffer; };
		virtual inline uint32_t getFramebufferIndex() { return 0; };

		void drawWorld(TimeStep interpolation_value, EntityRegistry& world, EntityId camera);

	protected:
		//TEMP
		Texture temp_texture = nullptr;
		Shader mesh_shader = nullptr;
		//END TEMP

		uint32_t draw_call_count = 0;
		bool use_frustum_culling = true;

		vector2U view_size;
		FramebufferLayout layout;

		Framebuffer framebuffer;
		MTCommandBuffer mt_command_buffer;

		CommandBufferInterface* command_buffer = nullptr;

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
		DynamicBuffer material_buffer = nullptr;//Dynamic for now
		Texture empty_texture = nullptr;
		Sampler basic_sampler = nullptr;

		//Object Transform: Push Constant
		struct ObjectTransformUniform
		{
			matrix4F model_matrix;
			glm::mat3x4 normal_matrix;
		};
	};
}
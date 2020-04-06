#pragma once

#include "Genesis/Resource/Material.hpp"
#include "Genesis/Entity/World.hpp"

namespace Genesis
{
	class WorldRenderer
	{
	public:
		WorldRenderer(RenderingBackend* backend);
		~WorldRenderer();

		void drawWorld(World* world);

	protected:
		uint32_t draw_call_count = 0;
		bool use_frustum_culling = true;

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
#pragma once

#include "Genesis/Ecs/Entt.hpp"
#include "Genesis/Rendering/Frustum.hpp"
#include "Genesis/Rendering/Lights.hpp"
#include "Genesis/Entity/World.hpp"

namespace Genesis
{
	struct SceneData
	{
		vector3F camera_position;
		vector3F ambient_light;
		matrix4F view_projection_matrix;
	};

	class MeshRenderer
	{
	public:
		virtual void drawAmbientPass(World* world, SceneData* environment, Frustum* frustum) = 0;

		virtual void drawAmbientPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum) = 0;

		virtual void drawDirectionalPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, DirectionalLight& light, TransformD& light_transform) = 0;
		virtual void drawPointPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, PointLight& light, TransformD& light_transform) = 0;
		virtual void drawSpotPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, SpotLight& light, TransformD& light_transform) = 0;
		
		//virtual void drawDirectionalShadowPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, DirectionalLight& light, ) = 0;
		//virtual void drawSpotShadowPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum) = 0;

		//virtual void drawShadowPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum) = 0;
	};
}
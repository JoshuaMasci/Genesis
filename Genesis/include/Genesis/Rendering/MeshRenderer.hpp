#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Entity/EntitySystem.hpp"
#include "Genesis/Rendering/Frustum.hpp"
#include "Genesis/Rendering/Lighting.hpp"
#include "Genesis/Core/Transform.hpp"

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
		virtual void drawAmbientPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum) = 0;

		virtual void drawDirectionalPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, DirectionalLight& light, TransformF& light_transform) = 0;
		virtual void drawPointPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, PointLight& light, TransformF& light_transform) = 0;
		virtual void drawSpotPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, SpotLight& light, TransformF& light_transform) = 0;
		
		//virtual void drawDirectionalShadowPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum, DirectionalLight& light, ) = 0;
		//virtual void drawSpotShadowPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum) = 0;

		//virtual void drawShadowPass(EntityRegistry* entity_registry, SceneData* environment, Frustum* frustum) = 0;
	};
}
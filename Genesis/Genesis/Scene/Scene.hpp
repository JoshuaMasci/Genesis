#pragma once

#include "Genesis/Core/List.hpp"

#include "Genesis/Scene/Mesh.hpp"
#include "Genesis/Scene/Lighting.hpp"
#include "Genesis/Scene/Camera.hpp"

namespace Genesis
{
	struct Scene
	{
		struct CameraTransform
		{
			Camera* camera;
			TransformF* transform;
		};

		struct MeshTransform
		{
			Mesh* mesh;
			TransformF* transform;
		};

		struct DirectionalLightTransform
		{
			DirectionalLight* directional_light;
			TransformF* transform;
		};

		struct PointLightTransform
		{
			PointLight* point_light;
			TransformF* transform;
		};

		struct SpotLightTransform
		{
			SpotLight* spot_light;
			TransformF* transform;
		};

		List<MeshTransform> meshes;
		List<DirectionalLightTransform> directional_lights;
		List<PointLightTransform> point_lights;
		List<SpotLightTransform> spot_lights;

		CameraTransform camera;
		vector3F ambient_light;
	};
}
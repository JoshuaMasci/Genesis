#pragma once

#include "Genesis/Component/ModelComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"

namespace Genesis
{
	struct ModelStruct
	{
		shared_ptr<MeshResource> mesh;
		shared_ptr<Material> material;
		TransformD transform;
	};

	struct DirectionalLightStruct
	{
		DirectionalLight light;
		TransformD transform;
	};

	struct PointLightStruct
	{
		PointLight light;
		TransformD transform;
	};

	struct SceneSettings
	{
		bool lighting = true;
		bool frustrum_culling = true;
	};

	struct SceneInfo
	{
		SceneSettings settings;

		Camera camera;
		TransformD camera_transform;

		vector<ModelStruct> models;

		vector3F ambient_light = vector3D(1.0);
		vector<DirectionalLightStruct> directional_lights;
		vector<PointLightStruct> point_lights;

		void clearBuffers()
		{
			models.clear();
			directional_lights.clear();
			point_lights.clear();
		}
	};
}
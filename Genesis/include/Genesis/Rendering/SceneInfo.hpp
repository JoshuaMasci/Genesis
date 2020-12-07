#pragma once

#include "Genesis/Component/ModelComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"

namespace Genesis
{
	struct CameraStruct
	{
		Camera camera;
		TransformD transform;
	};

	struct ModelStruct
	{
		shared_ptr<Mesh> mesh;
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

	struct SceneInfo
	{
		vector3F ambient_light = vector3D(1.0);

		vector<ModelStruct> models;
		vector<DirectionalLightStruct> directional_lights;
		vector<PointLightStruct> point_lights;

		void clear_buffers()
		{
			models.clear();
			directional_lights.clear();
			point_lights.clear();
		}
	};
}
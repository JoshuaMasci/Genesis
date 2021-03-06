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

	struct SpotLightStruct
	{
		SpotLight light;
		TransformD transform;
	};

	struct SceneRenderList
	{
		vector<ModelStruct> models;
		vector<DirectionalLightStruct> directional_lights;
		vector<PointLightStruct> point_lights;
		vector<SpotLightStruct> spot_lights;

		void clear()
		{
			models.clear();
			directional_lights.clear();
			point_lights.clear();
			spot_lights.clear();
		}
	};
}
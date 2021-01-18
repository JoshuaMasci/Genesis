#pragma once 

namespace Genesis
{
	struct SceneLightingSettings
	{
		vector3F ambient_light = vector3D(1.0);
		float gamma_correction = 2.2f;
		//TODO Skybox/EnvironmentMap
	};
}
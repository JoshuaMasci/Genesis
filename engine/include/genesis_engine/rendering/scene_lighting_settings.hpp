#pragma once 

namespace genesis_engine
{
	struct SceneLightingSettings
	{
		vec3f ambient_light = vec3d(1.0);
		float gamma_correction = 2.2f;
		//TODO Skybox/EnvironmentMap
	};
}
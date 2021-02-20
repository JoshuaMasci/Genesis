#pragma once

namespace genesis_engine
{
	struct BaseLight
	{
		bool enabled;
		vec3f color;
		float intensity;
	};

	struct DirectionalLight : BaseLight
	{
		DirectionalLight(vec3f color = vec3f(0.0f), float intensity = 0.0f, bool enabled = true)
		{
			this->enabled = enabled;

			this->color = color;
			this->intensity = intensity;
		};
	};

	struct PointLight : BaseLight
	{
		PointLight(float range = 0.0f, vec2f attenuation = vec2f(0.0f), vec3f color = vec3f(0.0f), float intensity = 0.0f, bool enabled = true)
		{
			this->enabled = enabled;

			this->color = color;
			this->intensity = intensity;

			this->range = range;
			this->attenuation = attenuation;
		}

		float range;
		vec2f attenuation;
	};

	struct SpotLight : PointLight
	{
		SpotLight(float cutoff = 0.0f, float range = 0.0f, vec2f attenuation = vec2f(0.0f), vec3f color = vec3f(0.0f), float intensity = 0.0f, bool enabled = true)
		{
			this->enabled = enabled;

			this->color = color;
			this->intensity = intensity;

			this->range = range;
			this->attenuation = attenuation;
			this->cutoff = cutoff;
		}

		float cutoff;
	};
}
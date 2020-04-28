#pragma once

#include "Genesis/Core/VectorTypes.hpp"

namespace Genesis
{
	struct BaseLight
	{
		bool enabled;
		vector3F color;
		float intensity;
	};

	struct ShadowCaster
	{
		bool shadow_map_enabled;
		vector2F shadow_map_size;
	};

	struct DirectionalLight : BaseLight
	{
		DirectionalLight(vector3F color = vector3F(0.0f), float intensity = 0.0f, bool enabled = true)
		{
			this->enabled = enabled;

			this->color = color;
			this->intensity = intensity;
		};
	};

	struct PointLight : BaseLight
	{
		PointLight(float range = 0.0f, vector2F attenuation = vector2F(0.0f), vector3F color = vector3F(0.0f), float intensity = 0.0f, bool enabled = true)
		{
			this->enabled = enabled;

			this->color = color;
			this->intensity = intensity;

			this->range = range;
			this->attenuation = attenuation;
		}

		float range;
		vector2F attenuation;
	};

	struct SpotLight : PointLight
	{
		SpotLight(float cutoff = 0.0f, float range = 0.0f, vector2F attenuation = vector2F(0.0f), vector3F color = vector3F(0.0f), float intensity = 0.0f, bool enabled = true)
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
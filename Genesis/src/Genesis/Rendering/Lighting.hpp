#pragma once

#include "Genesis/Core/VectorTypes.hpp"

namespace Genesis
{
	struct DirectionalLight
	{
		DirectionalLight(vector3F color = vector3F(0.0f), float intensity = 0.0f, bool casts_shadows = false, bool enabled = true)
		{
			this->enabled = enabled;

			this->color = color;
			this->intensity = intensity;

			this->casts_shadows = casts_shadows;
			this->shadow_size = vector2F(0.0);
		};

		bool enabled;

		vector3F color;
		float intensity;

		bool casts_shadows;
		vector2F shadow_size;
	};

	struct PointLight
	{
		PointLight(float range = 0.0f, vector2F attenuation = vector2F(0.0f), vector3F color = vector3F(0.0f), float intensity = 0.0f, bool enabled = true)
		{
			this->enabled = enabled;

			this->color = color;
			this->intensity = intensity;

			this->range = range;
			this->attenuation = attenuation;
		}

		bool enabled;

		vector3F color;
		float intensity;

		float range;
		vector2F attenuation;
	};

	struct SpotLight
	{
		SpotLight(float cutoff = 0.0f, float range = 0.0f, vector2F attenuation = vector2F(0.0f), vector3F color = vector3F(0.0f), float intensity = 0.0f, bool casts_shadows = false, bool enabled = true)
		{
			this->enabled = enabled;
			this->casts_shadows = casts_shadows;

			this->color = color;
			this->intensity = intensity;

			this->range = range;
			this->attenuation = attenuation;
			this->cutoff = cutoff;
		}

		bool enabled;
		bool casts_shadows;

		vector3F color;
		float intensity;

		float range;
		vector2F attenuation;
		float cutoff;
	};
}
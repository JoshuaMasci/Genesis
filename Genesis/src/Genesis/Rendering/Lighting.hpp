#pragma once

#include "Genesis/Core/VectorTypes.hpp"

namespace Genesis
{
	struct BaseLight
	{
		BaseLight(vector3F color, float intensity, bool casts_shadows = false, bool enabled = true)
		{
			this->color = color;
			this->intensity = intensity;
			this->casts_shadows = casts_shadows;
			this->enabled = enabled;
		}

		vector3F color;
		float intensity;

		bool casts_shadows;
		bool enabled;
	};

	struct DirectionalLight : public BaseLight
	{
		DirectionalLight(vector3F direction = vector3F(0.0), vector3F color = vector3F(0.0), float intensity = 0.0f, bool casts_shadows = false, bool enabled = true)
		:BaseLight(color, intensity, casts_shadows, enabled)
		{
			this->direction = direction;
			this->shadow_size = vector2F(0.0);
		};

		vector3F direction;
		vector2F shadow_size;
	};

	struct PointLight : public BaseLight
	{
		PointLight(float range, vector3F attenuation, vector3F color, float intensity, bool casts_shadows = false, bool enabled = true)
			:BaseLight(color, intensity, casts_shadows, enabled)
		{
			this->range = range;
			this->attenuation = attenuation;
		}

		float range;
		vector3F attenuation;
	};

	struct SpotLight : public PointLight
	{
		SpotLight(float cutoff, float range, vector3F attenuation, vector3F color, float intensity, bool casts_shadows = false, bool enabled = true)
			:PointLight(range, attenuation, color, intensity, casts_shadows, enabled)
		{
			this->cutoff = cutoff;
		}

		float cutoff;
	};
}
#pragma once 

namespace Genesis
{
	struct RenderSettings
	{
		bool lighting = true;
		bool frustrum_culling = true;
		float gamma_correction = 2.2f;
	};
}
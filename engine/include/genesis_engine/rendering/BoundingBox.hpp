#pragma once

namespace genesis_engine
{
	struct BoundingBox
	{
		BoundingBox() {};
		BoundingBox(vec3f min, vec3f max) : min(min), max(max) {};
		vec3f min;
		vec3f max;
	};

	struct AxisAlignedBoundingBox
	{
		AxisAlignedBoundingBox() {};
		AxisAlignedBoundingBox(vec3f min, vec3f max) : min(min), max(max) {};
		vec3f min;
		vec3f max;
	};
}
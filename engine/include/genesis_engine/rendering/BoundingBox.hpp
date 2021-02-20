#pragma once

namespace Genesis
{
	struct BoundingBox
	{
		BoundingBox() {};
		BoundingBox(vector3F min, vector3F max) : min(min), max(max) {};
		vector3F min;
		vector3F max;
	};

	struct AxisAlignedBoundingBox
	{
		AxisAlignedBoundingBox() {};
		AxisAlignedBoundingBox(vector3F min, vector3F max) : min(min), max(max) {};
		vector3F min;
		vector3F max;
	};
}
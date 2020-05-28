#pragma once

namespace Genesis
{
	struct AnimationChannel
	{
		enum PathType { TRANSLATION, ROTATION, SCALE };
		enum InterpolationType { LINEAR, STEP, CUBICSPLINE };

		string bone_name;
		PathType path;
		InterpolationType interpolation;
		vector<float> timesteps;
		vector<vector4F> values;
	};

	struct Animation
	{
		string name;
		vector<AnimationChannel> channels;
		float start_time = std::numeric_limits<float>::max();
		float end_time = std::numeric_limits<float>::min();
	};
}
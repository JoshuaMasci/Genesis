#pragma once

#define MAX_BONES 128

namespace Genesis
{
	struct Bone
	{
		string name;
		TransformF local_transform;
		Bone* parent_bone;
		vector<Bone*> child_bones;
	};

	struct Skeleton
	{
		string name;
		Bone* root_bone;
		map<string,Bone*> bone_hash;

		uint32_t bone_count;
		Bone bone_storage[128];
	};
}
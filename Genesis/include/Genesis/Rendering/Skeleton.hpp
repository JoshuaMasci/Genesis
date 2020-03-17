#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Core/List.hpp"
#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Core/Transform.hpp"

namespace Genesis
{
	typedef size_t BoneId;

	struct Bone
	{
		string name;
		BoneId id;
		matrix4F inverse_bind_transform; //Not changed after creation

		TransformF local_transform;

		List<Bone*> child_bones;
	};

	class Skeleton
	{
	public:
		Skeleton(size_t bone_count);
		~Skeleton();

		size_t getBoneCount();
		Bone* getBone(BoneId id);
		Bone* getBone(string name);

		void calcBoneMatrices(matrix4F* matrix_array);

	private:
		List<Bone> bones;
	};
}
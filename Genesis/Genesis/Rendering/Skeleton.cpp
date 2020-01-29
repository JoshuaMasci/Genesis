#include "Skeleton.hpp"

using namespace Genesis;

Skeleton::Skeleton(size_t bone_count)
{
	this->bones.resize(bone_count);
	for (size_t i = 0; i < this->bones.size(); i++)
	{
		this->bones[i].id = i;
	}
}

Skeleton::~Skeleton()
{

}

size_t Skeleton::getBoneCount()
{
	return this->bones.size();
}

Bone* Skeleton::getBone(BoneId id)
{
	return &this->bones[id];
}

Bone* Genesis::Skeleton::getBone(string name)
{
	for (size_t i = 0; i < this->bones.size(); i++)
	{
		if (this->bones[i].name == name)
		{
			return &this->bones[i];
		}
	}

	return nullptr;
}

#include <glm/gtx/string_cast.hpp>

void updateBone(Bone* bone, TransformF& parent_transform, matrix4F* matrix_array)
{
	if (bone == nullptr)
	{
		return;
	}

	TransformF global_transform = bone->local_transform.transformBy(parent_transform);

	matrix4F global_matrix = global_transform.getModelMatrix();

	global_matrix = bone->inverse_bind_transform * global_matrix;

	matrix_array[bone->id] = global_matrix;

	for (size_t i = 0; i < bone->child_bones.size(); i++)
	{
		updateBone(bone->child_bones[i], global_transform, matrix_array);
	}
}

void Skeleton::calcBoneMatrices(matrix4F* matrix_array)
{
	//Root is zero index?
	updateBone(&this->bones[0], TransformF(), matrix_array);
}

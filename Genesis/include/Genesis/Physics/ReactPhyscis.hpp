#pragma once

#pragma warning( push )
#pragma warning( disable : 4267 )
#pragma warning( disable : 4244 )
#include <reactphysics3d.h>
#pragma warning( pop )

namespace Genesis
{
	inline vector2D toVec2D(const reactphysics3d::Vector2& vec) { return vector2D(vec.x, vec.y); };
	inline vector3D toVec3D(const reactphysics3d::Vector3& vec) { return vector3D(vec.x, vec.y, vec.z); };
	inline quaternionD toQuatD(const reactphysics3d::Quaternion& quat) { return quaternionD(quat.w, quat.x, quat.y, quat.z); };
	//inline TransformD toTransformD(const reactphysics3d::Transform& trans) { return TransformD(toVec3D(trans.getPosition()), toQuatD(trans.getOrientation()), vector3D(1.0f)); };

	inline reactphysics3d::Vector2 toVec2R(const vector2D& vec) { return reactphysics3d::Vector2(vec.x, vec.y); };
	inline reactphysics3d::Vector3 toVec3R(const vector3D& vec) { return reactphysics3d::Vector3(vec.x, vec.y, vec.z); };
	inline reactphysics3d::Quaternion toQuatR(const quaternionD& quat) { return reactphysics3d::Quaternion(quat.x, quat.y, quat.z, quat.w); };
}
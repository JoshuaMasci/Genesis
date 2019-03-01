#include "Bullet_Include.hpp"

using namespace Genesis;

btVector3 Genesis::toBtVec3(const vector3D& vec)
{
	return btVector3(vec.x, vec.y, vec.z);
}

vector3D Genesis::toVec3(const btVector3& vec)
{
	return vector3D(vec.getX(), vec.getY(), vec.getZ());
}

btQuaternion Genesis::toBtQuat(const quaternionD& quat)
{
	return btQuaternion(quat.x, quat.y, quat.z, quat.w);
}

quaternionD Genesis::toQuat(const btQuaternion& quat)
{
	return quaternionD(quat.getW(), quat.getX(), quat.getY(), quat.getZ());
}

btTransform Genesis::toBtTransform(const Transform& trans)
{
	return btTransform(toBtQuat(trans.getOrientation()) ,toBtVec3(trans.getPosition()));
}

Transform Genesis::toTransform(const btTransform& trans)
{
	return Transform(toVec3(trans.getOrigin()), toQuat(trans.getRotation()));
}

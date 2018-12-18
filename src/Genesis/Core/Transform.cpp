#include "Transform.hpp"

using namespace genesis;

inline Transform::Transform(vector3D position, quaternionD orientation)
{
	this->position = position;
	this->orientation = orientation;
}

inline vector3D Transform::getPosition() const { return this->position; }
inline quaternionD Transform::getOrientation() const { return this->orientation; }

//Getters for the directional vectors.
inline vector3D Transform::getForward() const { return this->orientation * vector3D(0.0f, 0.0f, 1.0f); }
inline vector3D Transform::getUp() const { return this->orientation * vector3D(0.0f, 1.0f, 0.0f); }
inline vector3D Transform::getLeft() const { return this->orientation * vector3D(1.0f, 0.0f, 0.0f); }

inline void Transform::setPosition(const vector3D & vec) { this->position = vec; }
inline void Transform::setOrientation(const quaternionD & quat) { this->orientation = quat; }

inline void Transform::setTransform(const Transform & trans) { this->setPosition(trans.getPosition()); this->setOrientation(trans.getOrientation()); }

inline Transform Transform::transformBy(const Transform & transform1) const
{
	Transform result;
	result.setOrientation(transform1.getOrientation() * this->getOrientation());
	result.setPosition(transform1.getPosition() + (transform1.getOrientation() * this->getPosition()));
	return result;
}

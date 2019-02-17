#include "Transform.hpp"

using namespace Genesis;

Transform::Transform(vector3D position, quaternionD orientation)
{
	this->setPosition(position);
	this->setOrientation(orientation);
}

vector3D Transform::getPosition() const { return this->position; }
quaternionD Transform::getOrientation() const { return this->orientation; }

//Getters for the directional vectors.
vector3D Transform::getForward() const { return this->orientation * vector3D(0.0f, 0.0f, 1.0f); }
vector3D Transform::getUp() const { return this->orientation * vector3D(0.0f, 1.0f, 0.0f); }
vector3D Transform::getLeft() const { return this->orientation * vector3D(1.0f, 0.0f, 0.0f); }

void Transform::setPosition(const vector3D& vec) 
{ 
	this->position = vec;
}
void Transform::setOrientation(const quaternionD& quat) 
{ 
	this->orientation = quat;
	this->updateModelMatrix();
}

void Transform::setTransform(const Transform& trans) { this->setPosition(trans.getPosition()); this->setOrientation(trans.getOrientation()); }

Transform Transform::transformBy(const Transform& transform1) const
{
	Transform result;
	result.setOrientation(transform1.getOrientation() * this->getOrientation());
	result.setPosition(transform1.getPosition() + (transform1.getOrientation() * this->getPosition()));
	return result;
}

void Genesis::Transform::updateModelMatrix()
{
	this->model_matrix = glm::toMat4((quaternionF)this->orientation);//Scale does not exist right now
}

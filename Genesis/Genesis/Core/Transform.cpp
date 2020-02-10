#include "Transform.hpp"

using namespace Genesis;

TransformF::TransformF(vector3F& position, quaternionF& orientation, vector3F& scale)
{
	this->setPosition(position);
	this->setOrientation(orientation);
	this->setScale(scale);
}

TransformF TransformF::transformBy(const TransformF& transform1) const
{
	TransformF result;
	result.position = transform1.position + (transform1.orientation * (this->position * transform1.scale));
	result.orientation = transform1.orientation * this->orientation;
	result.scale = transform1.scale * this->scale;
	return result;
}

matrix4F TransformF::getModelMatrix() const
{
	matrix4F translate = glm::translate(matrix4F(1.0f), this->position);
	matrix4F orientation = glm::toMat4(this->orientation);
	matrix4F scale = glm::scale(matrix4F(1.0f), this->scale);
	return translate * orientation * scale;
}

matrix4F Genesis::TransformF::getViewMatirx() const
{
	return glm::lookAt(this->position, this->position + this->getForward(), this->getUp());
}

matrix3F TransformF::getNormalMatrix() const
{
	return glm::toMat3(this->orientation);
}

TransformD::TransformD(vector3D position, quaternionD orientation, vector3D scale)
{
	this->setPosition(position);
	this->setOrientation(orientation);
	this->setScale(scale);
}

TransformD TransformD::transformBy(const TransformD& transform1) const
{
	TransformD result;
	result.position = transform1.position + (transform1.orientation * (this->position * transform1.scale));
	result.orientation = transform1.orientation * this->orientation;
	result.scale = transform1.scale * this->scale;
	return result;
}

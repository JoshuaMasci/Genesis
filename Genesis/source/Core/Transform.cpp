#include "Genesis/Core/Transform.hpp"

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

matrix4F TransformF::getViewMatirx() const
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
	this->updateModelMatrix();
}

TransformD TransformD::transformBy(const TransformD& transform1) const
{
	return TransformD(transform1.position + (transform1.orientation * (this->position * transform1.scale)), transform1.orientation * this->orientation, transform1.scale * this->scale);
}

matrix4F TransformD::getModelMatrix(const vector3D& position_offset)
{
	if (this->has_changed == true)
	{
		this->updateModelMatrix();
	}

	matrix4F translated_matrix = glm::translate(matrix4F(1.0f), (vector3F)(this->position - position_offset));
	/*matrix4F translated_matrix = this->untranslated_model_matrix;
	vector3F translated_position = (vector3F)(this->position - pos_offset);
	translated_matrix[0][3] = translated_position.x;
	translated_matrix[1][3] = translated_position.y;
	translated_matrix[2][3] = translated_position.z;*/
	return translated_matrix * this->untranslated_model_matrix;
}

matrix3F TransformD::getNormalMatrix()
{
	if (this->has_changed == true)
	{
		this->updateModelMatrix();
	}
	return matrix3F(glm::transpose(glm::inverse(this->untranslated_model_matrix)));
}

matrix4F TransformD::getViewMatirx(const vector3D& position_offset)
{
	vector3F eye_pos = (vector3F)(vector3F)(this->position - position_offset);
	vector3F center = eye_pos + (vector3F)this->getForward();
	vector3F up = (vector3F)this->getUp();
	return glm::lookAt(eye_pos, center, up);
}

void TransformD::updateModelMatrix()
{
	matrix4F orientation = glm::toMat4(this->orientation);
	matrix4F scale = glm::scale(matrix4F(1.0f), (vector3F)this->scale);
	this->untranslated_model_matrix = orientation * scale;
	this->has_changed = false;
}

TransformD TransformUtil::transformBy(const TransformD& parent, const TransformD& child)
{
	TransformD result;
	result.setPosition(parent.getPosition() + (parent.getOrientation() * (child.getPosition() * parent.getScale())));
	result.setOrientation(parent.getOrientation() * child.getOrientation());
	result.setScale(parent.getScale() * child.getScale());
	return result;
}

TransformD TransformUtil::transformBy(const TransformD& parent, const TransformF& child)
{
	TransformD result;
	result.setPosition(parent.getPosition() + (parent.getOrientation() * ((vector3D)child.getPosition() * parent.getScale())));
	result.setOrientation(parent.getOrientation() * (quaternionD)child.getOrientation());
	result.setScale(parent.getScale() * (vector3D)child.getScale());
	return result;
}

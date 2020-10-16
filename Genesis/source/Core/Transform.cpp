#include "Genesis/Core/Transform.hpp"

namespace Genesis
{
	TransformF::TransformF(vector3F& position, quaternionF& orientation, vector3F& scale)
	{
		this->setPosition(position);
		this->setOrientation(orientation);
		this->setScale(scale);
	}

	void TransformF::setTransform(const matrix4F& matrix)
	{
		this->position = vector3D(matrix[3].x, matrix[3].y, matrix[3].z);

		vector3F temp_scale;
		temp_scale.x = glm::length(vector3F(matrix[0][0], matrix[0][1], matrix[0][2]));
		temp_scale.y = glm::length(vector3F(matrix[1][0], matrix[1][1], matrix[1][2]));
		temp_scale.z = glm::length(vector3F(matrix[2][0], matrix[2][1], matrix[2][2]));
		this->scale = temp_scale;

		this->orientation = glm::toQuat(matrix3F(matrix));
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

	matrix4F TransformD::getModelMatrix(const vector3D& position_offset)
	{
		if (this->dirty == true)
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
		if (this->dirty == true)
		{
			this->updateModelMatrix();
		}
		;
		return glm::transpose(glm::inverse(matrix3F(this->untranslated_model_matrix)));
	}

	matrix4F TransformD::getViewMatirx(const vector3D& position_offset)
	{
		vector3F eye_pos = (vector3F)(vector3F)(this->position - position_offset);
		vector3F center = eye_pos + (vector3F)this->getForward();
		vector3F up = (vector3F)this->getUp();
		return glm::lookAt(eye_pos, center, up);
	}

	void TransformD::setTransform(const TransformD& new_transform)
	{
		this->position = new_transform.position;
		this->orientation = new_transform.orientation;
		this->scale = new_transform.scale;

		this->untranslated_model_matrix = new_transform.untranslated_model_matrix;
		this->dirty = new_transform.dirty;
	}

	void TransformD::updateModelMatrix()
	{
		matrix4F orientation = glm::toMat4(this->orientation);
		matrix4F scale = glm::scale(matrix4F(1.0f), (vector3F)this->scale);
		this->untranslated_model_matrix = orientation * scale;
		this->dirty = false;
	}

	void TransformUtils::transformByInplace(TransformF& destination, const TransformF& origin, const TransformF& local)
	{
		destination.setPosition(origin.getPosition() + (origin.getOrientation() * (local.getPosition() * origin.getScale())));
		destination.setOrientation(origin.getOrientation() * local.getOrientation());
		destination.setScale(origin.getScale() * local.getScale());
	}

	void TransformUtils::transformByInplace(TransformD& destination, const TransformD& origin, const TransformF& local)
	{
		destination.setPosition(origin.getPosition() + (origin.getOrientation() * ((vector3D)local.getPosition() * origin.getScale())));
		destination.setOrientation(origin.getOrientation() * (quaternionD)local.getOrientation());
		destination.setScale(origin.getScale() * (vector3D)local.getScale());
	}

	void TransformUtils::transformByInplace(TransformD& destination, const TransformD& origin, const TransformD& local)
	{
		destination.setPosition(origin.getPosition() + (origin.getOrientation() * (local.getPosition() * origin.getScale())));
		destination.setOrientation(origin.getOrientation() * local.getOrientation());
		destination.setScale(origin.getScale() * local.getScale());
	}

	TransformD TransformUtils::transformBy(const TransformD& origin, const TransformF& local)
	{
		TransformD transform;
		transform.setPosition(origin.getPosition() + (origin.getOrientation() * ((vector3D)local.getPosition() * origin.getScale())));
		transform.setOrientation(origin.getOrientation() * (quaternionD)local.getOrientation());
		transform.setScale(origin.getScale() * (vector3D)local.getScale());
		return transform;
	}

	TransformD TransformUtils::transformBy(const TransformD& origin, const TransformD& local)
	{
		TransformD transform;
		transform.setPosition(origin.getPosition() + (origin.getOrientation() * (local.getPosition() * origin.getScale())));
		transform.setOrientation(origin.getOrientation() * local.getOrientation());
		transform.setScale(origin.getScale() * local.getScale());
		return transform;
	}

	void TransformUtils::untransformByInplace(TransformD& destination, const TransformD& origin, const TransformD& global)
	{
		destination.setPosition((glm::inverse(origin.getOrientation()) * (global.getPosition() - origin.getPosition())) / origin.getScale());
		destination.setOrientation(glm::inverse(origin.getOrientation()) * global.getOrientation());
		destination.setScale(global.getScale() / origin.getScale());
	}

	TransformD TransformUtils::toTransformD(const TransformF& transform)
	{
		return TransformD((vector3D) transform.getPosition(), (quaternionD) transform.getOrientation(), (vector3D) transform.getScale());
	}
}
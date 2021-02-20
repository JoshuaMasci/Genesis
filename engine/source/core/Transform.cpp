#include "genesis_engine/core/Transform.hpp"

namespace genesis_engine
{
	TransformF::TransformF(vec3f& position, quatf& orientation, vec3f& scale)
	{
		this->setPosition(position);
		this->setOrientation(orientation);
		this->setScale(scale);
	}

	void TransformF::setTransform(const mat4f& matrix)
	{
		this->position = vec3d(matrix[3].x, matrix[3].y, matrix[3].z);

		vec3f temp_scale;
		temp_scale.x = glm::length(vec3f(matrix[0][0], matrix[0][1], matrix[0][2]));
		temp_scale.y = glm::length(vec3f(matrix[1][0], matrix[1][1], matrix[1][2]));
		temp_scale.z = glm::length(vec3f(matrix[2][0], matrix[2][1], matrix[2][2]));
		this->scale = temp_scale;

		this->orientation = glm::toQuat(mat3f(matrix));
	}

	mat4f TransformF::getModelMatrix() const
	{
		mat4f translate = glm::translate(mat4f(1.0f), this->position);
		mat4f orientation = glm::toMat4(this->orientation);
		mat4f scale = glm::scale(mat4f(1.0f), this->scale);
		return translate * orientation * scale;
	}

	mat4f TransformF::getViewMatirx() const
	{
		return glm::lookAt(this->position, this->position + this->getForward(), this->getUp());
	}

	mat3f TransformF::getNormalMatrix() const
	{
		return glm::toMat3(this->orientation);
	}

	TransformD::TransformD(vec3d position, quatd orientation, vec3d scale)
	{
		this->setPosition(position);
		this->setOrientation(orientation);
		this->setScale(scale);
		//this->updateModelMatrix();
	}

	mat4f TransformD::getModelMatrix(const vec3d& position_offset)
	{
		mat4f translation = glm::translate(mat4f(1.0f), (vec3f)(this->position - position_offset));
		mat4f orientation = glm::toMat4(this->orientation);
		mat4f scale = glm::scale(mat4f(1.0f), (vec3f)this->scale);
		return translation * orientation * scale;
	}

	mat3f TransformD::getNormalMatrix()
	{
		mat4f orientation = glm::toMat4(this->orientation);
		mat4f scale = glm::scale(mat4f(1.0f), (vec3f)this->scale);
		return glm::transpose(glm::inverse(mat3f(orientation * scale)));
	}

	mat4f TransformD::getViewMatirx(const vec3d& position_offset)
	{
		vec3f eye_pos = (vec3f)(vec3f)(this->position - position_offset);
		vec3f center = eye_pos + (vec3f)this->getForward();
		vec3f up = (vec3f)this->getUp();
		return glm::lookAt(eye_pos, center, up);
	}

	void TransformD::setTransform(const TransformD& new_transform)
	{
		this->position = new_transform.position;
		this->orientation = new_transform.orientation;
		this->scale = new_transform.scale;
	}

	bool TransformD::operator==(const TransformD& other)
	{
		return (this->position == other.position) && (this->orientation == other.orientation) && (this->scale == other.scale);
	}

	bool TransformD::operator!=(const TransformD& other)
	{
		return (this->position != other.position) || (this->orientation != other.orientation) || (this->scale != other.scale);
	}

	void TransformUtils::transformByInplace(TransformF& destination, const TransformF& origin, const TransformF& local)
	{
		destination.setPosition(origin.getPosition() + (origin.getOrientation() * (local.getPosition() * origin.getScale())));
		destination.setOrientation(origin.getOrientation() * local.getOrientation());
		destination.setScale(origin.getScale() * local.getScale());
	}

	void TransformUtils::transformByInplace(TransformD& destination, const TransformD& origin, const TransformF& local)
	{
		destination.setPosition(origin.getPosition() + (origin.getOrientation() * ((vec3d)local.getPosition() * origin.getScale())));
		destination.setOrientation(origin.getOrientation() * (quatd)local.getOrientation());
		destination.setScale(origin.getScale() * (vec3d)local.getScale());
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
		transform.setPosition(origin.getPosition() + (origin.getOrientation() * ((vec3d)local.getPosition() * origin.getScale())));
		transform.setOrientation(origin.getOrientation() * (quatd)local.getOrientation());
		transform.setScale(origin.getScale() * (vec3d)local.getScale());
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
		return TransformD((vec3d) transform.getPosition(), (quatd) transform.getOrientation(), (vec3d) transform.getScale());
	}
}
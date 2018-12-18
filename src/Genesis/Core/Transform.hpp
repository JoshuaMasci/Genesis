#pragma once

#include "Genesis/Core/Types.hpp"

namespace genesis
{
	class Transform
	{
	public:
		Transform(vector3D position = vector3D(0.0), quaternionD orientation = quaternionD(1.0, 0.0, 0.0, 0.0));;

		//Getters
		inline vector3D getPosition() const;;
		inline quaternionD getOrientation() const;;
		inline vector3D getForward() const;;
		inline vector3D getUp() const;;
		inline vector3D getLeft() const;;

		//Setters
		inline void setPosition(const vector3D& vec);;
		inline void setOrientation(const quaternionD& quat);;
		inline void setTransform(const Transform& trans);;

		//Utils
		Transform transformBy(const Transform&  transform1) const;;

	private:
		vector3D position;
		quaternionD orientation;
	};
};
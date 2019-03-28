#pragma once

#include "Genesis/Core/VectorTypes.hpp"

namespace Genesis
{
	class Transform
	{
	public:
		Transform(vector3D position = vector3D(0.0), quaternionD orientation = quaternionD(1.0, 0.0, 0.0, 0.0));

		//Getters
		vector3D getPosition() const;
		quaternionD getOrientation() const;
		vector3D getForward() const;
		vector3D getUp() const;
		vector3D getLeft() const;

		//Setters
		void setPosition(const vector3D& vec);
		void setOrientation(const quaternionD& quat);
		void setTransform(const Transform& trans);

		//Utils
		Transform transformBy(const Transform& transform1) const;

	private:
		//void updateModelMatrix();

		vector3D position;
		quaternionD orientation;
		
		//matrix4F model_matrix;
	};
};
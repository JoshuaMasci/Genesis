#pragma once

#include "Genesis/Physics/Bullet_Include.hpp"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	enum CollisionShapeType
	{
		Box,
		Capsule,
		ConvexMesh,
		ConcaveMesh,
	};

	class CollisionShape
	{
	public:

		btCollisionShape* getShape();

	private:
		void setBox(vector3D half_length);
		void setCapsule(double radius, double height);
		void setConvexMesh(string file_path);
		void setConcaveMesh(string file_path);

		CollisionShapeType shape_type;
		btCollisionShape* shape = nullptr;

		int index;
	};
};

#include "CollisionShape.hpp"

using namespace Genesis;

CollisionShape::CollisionShape(CollisionShapeType type, vector3D vec)
{
	if (type == CollisionShapeType::Box)
	{
		this->setBox(vec);
	}
}

CollisionShape::CollisionShape(CollisionShapeType type, vector2D vec)
{
	if (type == CollisionShapeType::Capsule)
	{
		this->setCapsule(vec.x, vec.y);
	}
}

CollisionShape::CollisionShape(CollisionShapeType type, string string)
{
	if (type == CollisionShapeType::ConvexMesh)
	{
		this->setConvexMesh(string);
	}
	else if (type == CollisionShapeType::ConcaveMesh)
	{
		this->setConcaveMesh(string);
	}
}

CollisionShape::~CollisionShape()
{
	if (this->shape != nullptr)
	{
		delete this->shape;
	}
}

void CollisionShape::setBox(vector3D half_length)
{
	if (this->shape != nullptr)
	{
		delete this->shape;
	}

	this->shape_type = CollisionShapeType::Box;
	this->shape = new btBoxShape(toBtVec3(half_length));
}

void CollisionShape::setCapsule(double radius, double height)
{
	if (this->shape != nullptr)
	{
		delete this->shape;
	}

	this->shape_type = CollisionShapeType::Capsule;
	this->shape = new btCapsuleShape(radius, height);
}

void CollisionShape::setConvexMesh(string file_path)
{
	if (this->shape != nullptr)
	{
		delete this->shape;
	}

	this->shape_type = CollisionShapeType::ConvexMesh;
	this->shape = nullptr;
}

void CollisionShape::setConcaveMesh(string file_path)
{

	if (this->shape != nullptr)
	{
		delete this->shape;
	}

	this->shape_type = CollisionShapeType::ConcaveMesh;
	this->shape = nullptr;
}

btCollisionShape* CollisionShape::getShape()
{
	return this->shape;
}

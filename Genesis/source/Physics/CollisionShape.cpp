#include "Genesis/Physics/CollisionShape.hpp"

#include "Genesis/Entity/Entity.hpp"
#include "Genesis/Entity/World.hpp"
#include "Genesis/Physics/Rigidbody.hpp"

using namespace Genesis;

CollisionShapeComponent::CollisionShapeComponent(Entity* entity, reactphysics3d::CollisionShape* shape)
	:Component(entity), collision_shape(shape)
{
	if (this->parent_entity->getWorld() != nullptr)
	{
		this->addtoWorld(this->parent_entity->getWorld());
	}
}

CollisionShapeComponent::~CollisionShapeComponent()
{
	this->removeFromWorld();
	delete this->collision_shape;
}

void CollisionShapeComponent::addtoWorld(World* world)
{
	Entity* root = this->parent_entity->getRoot();
	if (root->hasRigidbody())
	{
		this->proxy_shape = root->getRigidbody()->addCollisionShape(this->collision_shape, this->parent_entity->getRootTransform());
		this->proxy_shape->setUserData(this->parent_entity);
	}
}

void CollisionShapeComponent::removeFromWorld()
{
	if (this->proxy_shape == nullptr)
	{
		return;
	}

	Entity* root = this->parent_entity->getRoot();
	if (root->hasRigidbody())
	{
		root->getRigidbody()->removeCollisionShape(this->proxy_shape);
		this->proxy_shape = nullptr;
	}
}

void CollisionShapeComponent::onUpdate(TimeStep time_step)
{
}

void CollisionShapeComponent::onTransformUpdate()
{
	if (this->proxy_shape != nullptr)
	{
		TransformD transform = this->parent_entity->getRootTransform();
		this->proxy_shape->setLocalToBodyTransform(reactphysics3d::Transform(toVec3R(transform.getPosition()), toQuatR(transform.getOrientation())));
	}
}

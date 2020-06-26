#include "Genesis/Physics/CollisionComponent.hpp"

#include "Genesis/World/World.hpp"
#include "Genesis/Entity/Entity.hpp"
#include "Genesis/Physics/Rigidbody.hpp"

namespace Genesis
{
	CollisionComponent::CollisionComponent(Node* node, reactphysics3d::CollisionShape* shape)
	{
		this->parent = node;
		this->shape = shape;
	}

	CollisionComponent::~CollisionComponent()
	{
		this->removeFromWorld();
		delete this->shape;
	}

	void CollisionComponent::updateShape(reactphysics3d::CollisionShape* shape)
	{
		if (this->proxy_shape != nullptr)
		{
			this->removeFromWorld();
		}

		if (this->shape != nullptr)
		{
			delete this->shape;
		}

		this->shape = shape;

		this->addtoWorld(this->parent->getRootEntity()->getWorld());
	}

	void CollisionComponent::updateTransform(const TransformF& transform)
	{
		if (this->proxy_shape != nullptr)
		{
			reactphysics3d::Transform new_transform;
			new_transform.setPosition(toVec3R(transform.getPosition()));
			new_transform.setOrientation(toQuatR(transform.getOrientation()));
			this->proxy_shape->setLocalToBodyTransform(new_transform);
		}
	}

	void CollisionComponent::addtoWorld(World* world)
	{
		if (this->shape != nullptr)
		{
			if (this->parent->getRootEntity()->hasRigidbody())
			{
				this->proxy_shape = this->parent->getRootEntity()->getRigidbody()->addCollisionShape(this->shape, this->parent->getRootTransform());
				this->proxy_shape->setUserData(this->parent);
			}
		}
	}

	void CollisionComponent::removeFromWorld()
	{
		if (this->shape != nullptr)
		{
			if (this->proxy_shape != nullptr)
			{
				if (this->parent->getRootEntity()->hasRigidbody())
				{
					this->parent->getRootEntity()->getRigidbody()->removeCollisionShape(this->proxy_shape);
				}
			}
		}
	}
}
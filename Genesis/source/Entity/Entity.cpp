#include "Genesis/Entity/Entity.hpp"

#include "Genesis/Entity/World.hpp"
#include "Genesis/Physics/Rigidbody.hpp"

using namespace Genesis;

Entity::Entity(EntityId id)
{
	this->id = id;
	this->name = "Entity_" + std::to_string(id);

	//Assume this will be the root
	this->root = this;
}

Entity::Entity(EntityId id, string name)
{
	this->id = id;
	this->name = name;

	//Assume this will be the root
	this->root = this;
}

Entity::~Entity()
{
	for (Entity* child : this->children)
	{
		delete child;
	}

	for (auto component : this->component_map)
	{
		delete component.second;
	}

	delete this->rigidbody;
}

void Entity::updateTransform(TransformDirtyFlag parent_flag)
{
	TransformDirtyFlag entity_flag = TransformDirtyFlag::None;

	if (this->isRoot())
	{
		//Update rigidbody position
		if (this->rigidbody != nullptr && this->rigidbody->getHandle() != nullptr)
		{
			TransformD transform = this->rigidbody->getTransform();
			if (transform.getPosition() != this->local_transform.getPosition())
			{
				this->setLocalPosition(transform.getPosition());
			}

			if (transform.getOrientation() != this->local_transform.getOrientation())
			{
				this->setLocalOrientation(transform.getOrientation());
			}
		}

		if (this->transform_dirty)
		{
			this->global_transform = this->local_transform;
			entity_flag = TransformDirtyFlag::Global;
			this->transform_dirty = false;

			//if scale changed
			if (this->local_transform.getScale() != this->root_transform.getScale())
			{
				this->root_transform = TransformD(vector3D(0.0), quaternionD(1.0, 0.0, 0.0, 0.0), this->local_transform.getScale());

				//Force update of all root transforms
				entity_flag = TransformDirtyFlag::All;
			}
		}
	}
	else
	{
		if (this->transform_dirty || parent_flag == TransformDirtyFlag::All)
		{
			this->global_transform.transformByInplace(this->parent->global_transform, this->local_transform);
			this->root_transform.transformByInplace(this->parent->root_transform, this->local_transform);
			entity_flag = TransformDirtyFlag::All;
			this->transform_dirty = false;
		}
		else if (parent_flag == TransformDirtyFlag::Global)
		{
			this->global_transform.transformByInplace(this->parent->global_transform, this->local_transform);
			entity_flag = TransformDirtyFlag::Global;
		}	
	}

	if (entity_flag != TransformDirtyFlag::None)
	{
		this->onTransformUpdate(entity_flag);
	}

	for (Entity* child : this->children)
	{
		child->updateTransform(entity_flag);
	}
}

void Entity::onTransformUpdate(TransformDirtyFlag transform_dirty)
{
	//GENESIS_ENGINE_INFO("{}:{} transform dirty {}", this->id, this->name, transform_dirty);

	for (auto component : this->component_map)
	{
		component.second->onTransformUpdate();
	}

	if (this->rigidbody != nullptr)
	{
		this->rigidbody->setTransform(this->local_transform);
	}
}

void Entity::addChild(Entity* child)
{
	GENESIS_ENGINE_ASSERT_ERROR(child != nullptr, ("{}:{} tried to add a null child", this->id, this->name));
	GENESIS_ENGINE_ASSERT_ERROR(child->parent == nullptr && child->world == nullptr, ("{}:{} already has a parent", child->id, child->name));

	child->root = this->root;
	child->parent = this;
	this->children.push_back(child);

	if (this->world != nullptr)
	{
		child->addtoWorld(this->world);
	}
}

void Entity::removeChild(Entity* child)
{
	GENESIS_ENGINE_ASSERT_ERROR(child != nullptr, ("{}:{} tried to remove a null child", this->id, this->name));

	for (size_t i = 0; i < this->children.size(); i++)
	{
		if (this->children[i] == child)
		{
			size_t last_index = this->children.size() - 1;
			if (i != last_index)
			{
				this->children[i] = this->children[last_index];
			}
			this->children.pop_back();
			if (this->world != nullptr)
			{
				child->removeFromWorld();
			}
			child->root = nullptr;
			child->parent = nullptr;
			return;
		}
	}

	GENESIS_ENGINE_ERROR("{}:{} is not a child of {}:{}", child->id, child->name, this->id, this->name);
}

void Entity::createRigidbody()
{
	GENESIS_ENGINE_ASSERT_ERROR(rigidbody == nullptr, ("{}:{} already has a rigidbody", this->id, this->name));
	this->rigidbody = new Rigidbody();

	if (this->world != nullptr)
	{
		this->world->getPhysicsWorld()->addEntity(this);
	}
}

void Entity::removeRigidbody()
{
	GENESIS_ENGINE_ASSERT_ERROR(rigidbody != nullptr, ("{}:{} doesn't has a rigidbody", this->id, this->name));

	if (this->world != nullptr)
	{
		this->world->getPhysicsWorld()->removeEntity(this);
	}
}

void Entity::addtoWorld(World* world)
{
	GENESIS_ENGINE_ASSERT_ERROR(world != nullptr, ("{}:{} tried to join a null world", this->id, this->name));
	GENESIS_ENGINE_ASSERT_ERROR(this->world == nullptr, ("{}:{} already in a world", this->id, this->name));
	this->world = world;
	this->transform_dirty = true;

	if (this->rigidbody != nullptr)
	{
		this->world->getPhysicsWorld()->addEntity(this);
	}

	for (auto component : this->component_map)
	{
		component.second->addtoWorld(world);
	}

	for (Entity* child : this->children)
	{
		child->addtoWorld(this->world);
	}

}

void Entity::removeFromWorld()
{
	GENESIS_ENGINE_ASSERT_ERROR(this->world != nullptr, ("{}:{} not in a world", this->id, this->name));

	for (Entity* child : this->children)
	{
		child->removeFromWorld();
	}

	for (auto component : this->component_map)
	{
		component.second->removeFromWorld();
	}

	if (this->rigidbody != nullptr)
	{
		this->world->getPhysicsWorld()->removeEntity(this);
	}

	this->world = nullptr;
}

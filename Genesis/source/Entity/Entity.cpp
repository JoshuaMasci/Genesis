#include "Genesis/Entity/Entity.hpp"

#include "Genesis/Entity/World.hpp"

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
}

void Entity::updateTransform(bool has_parent_changed)
{
	if (has_parent_changed || this->local_transform_dirty)
	{
		if (this->parent != nullptr)
		{
			this->global_transform = this->local_transform.transformBy(this->parent->global_transform);
		}
		else
		{
			this->global_transform = this->local_transform;
		}

		for (Entity* child : this->children)
		{
			child->updateTransform(true);
		}

		this->local_transform_dirty = false;
	}
	else
	{
		for (Entity* child : this->children)
		{
			child->updateTransform(false);
		}
	}
}

void Entity::addChild(Entity* child)
{
	GENESIS_ENGINE_ASSERT_ERROR((child != nullptr), ("{}:{} tried to add a null child", this->id, this->name));
	GENESIS_ENGINE_ASSERT_ERROR((child->parent == nullptr && child->world == nullptr), ("{}:{} already has a parent", child->id, child->name));

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
	GENESIS_ENGINE_ASSERT_ERROR((child != nullptr), ("{}:{} tried to remove a null child", this->id, this->name));

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

void Entity::addtoWorld(World* world)
{
	GENESIS_ENGINE_ASSERT_ERROR((world != nullptr), ("{}:{} tried to join a null world", this->id, this->name));
	GENESIS_ENGINE_ASSERT_ERROR((this->world == nullptr), ("{}:{} already in a world", this->id, this->name));
	this->world = world;
	this->local_transform_dirty = true;

	for (Entity* child : this->children)
	{
		child->addtoWorld(this->world);
	}
}

void Entity::removeFromWorld()
{
	GENESIS_ENGINE_ASSERT_ERROR((this->world != nullptr), ("{}:{} not in a world", this->id, this->name));
	this->world = nullptr;

	for (Entity* child : this->children)
	{
		child->removeFromWorld();
	}
}

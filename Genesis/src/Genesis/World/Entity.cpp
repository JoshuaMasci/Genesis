#include "Entity.hpp"

#include "Genesis/World/ChildEntity.hpp"

using namespace Genesis;

Entity::Entity(EntityManager* manager, EntityId id)
	:manager(manager), entityId(id)
{
}

Entity::~Entity()
{
	for (int i = 0; i < this->child_entities.size(); i++)
	{
		if (this->child_entities[i] == nullptr)
		{
			//TODO: Mark for deletion
			//delete this->child_entities[i];
		}
	}
}

void Entity::addToWorld(World* new_world)
{
	if (this->world == nullptr)
	{
		this->world = new_world;

		for (int i = 0; i < this->child_entities.size(); i++)
		{
			if (this->child_entities[i] != nullptr)
			{
				this->child_entities[i]->addToWorld(new_world);
			}
		}
	}
	else
	{
		printf("Error: Already in world\n");
	}
}

void Entity::removeFromWorld()
{
	if (this->world != nullptr)
	{
		this->world = nullptr;

		for (int i = 0; i < this->child_entities.size(); i++)
		{
			if (this->child_entities[i] != nullptr)
			{
				this->child_entities[i]->removeFromWorld();
			}
		}
	}
	else
	{
		printf("Error: Not in world\n");
	}
}

Transform Entity::getLocalTransform()
{
	return this->local_transform;
}

void Entity::setLocalTransform(Transform& transform)
{
	this->local_transform = transform;
	//Mark for update
	local_transform_changed = true;
}

void Entity::addChild(Entity* child)
{
	child->parent_entity = this;
	child->root_entity = this->root_entity;

	for (int i = 0; i < this->child_entities.size(); i++)
	{
		if (this->child_entities[i] == nullptr)
		{
			this->child_entities[i] = child;
			return;
		}
	}

	this->child_entities.push_back(child);
}

void Entity::removeChild(Entity* child)
{
	for (int i = 0; i < this->child_entities.size(); i++)
	{
		if (this->child_entities[i] == child)
		{
			this->child_entities[i] = nullptr;
			child->parent_entity = nullptr;
			child->root_entity = nullptr;
			return;
		}
	}
}

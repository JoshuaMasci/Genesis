#include "RootEntity.hpp"

using namespace Genesis;

RootEntity::RootEntity(EntityManager* manager, EntityId id)
	:Entity(manager, id)
{
}

void RootEntity::recalculateTransform()
{
	this->relative_transform = Transform();
	this->world_transform = this->local_transform;

	for (int i = 0; i < this->child_entities.size(); i++)
	{
		if (this->child_entities[i] != nullptr)
		{
			this->child_entities[i]->recalculateTransform();
		}
	}
}
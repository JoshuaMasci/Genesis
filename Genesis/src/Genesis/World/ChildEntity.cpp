#include "ChildEntity.hpp"

using namespace Genesis;

ChildEntity::ChildEntity(EntityManager* manager, EntityId id)
	:Entity(manager, id)
{
}

void ChildEntity::recalculateTransform()
{
	if (this->parent_entity != nullptr)
	{
		this->relative_transform = this->parent_entity->getRelativeTransform().transformBy(this->local_transform);
		this->world_transform = this->parent_entity->getWorldTransform().transformBy(this->local_transform);
	}
	else
	{
		this->relative_transform = Transform();
		this->world_transform = Transform();
	}

	for (int i = 0; i < this->child_entities.size(); i++)
	{
		if (this->child_entities[i] != nullptr)
		{
			this->child_entities[i]->recalculateTransform();
		}
	}
}

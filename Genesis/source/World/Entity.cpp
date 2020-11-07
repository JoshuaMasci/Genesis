#include "Genesis/World/Entity.hpp"

namespace Genesis
{
	namespace Experimental
	{
		Entity::Entity(size_t id, const char* name)
			:id(id), name(name)
		{

		}

		Entity::~Entity()
		{
			for (auto component_ptr : this->components)
			{
				delete component_ptr.second;
			}

			for (auto child : this->children)
			{
				delete child;
			}
		}

		size_t Entity::getId()
		{
			return this->id;
		}

		char* Entity::getName()
		{
			return this->name.data;
		}

		TransformF& Entity::getLocalTransform()
		{
			return this->local_transform;
		}

		void Entity::addChild(Entity* entity)
		{
			entity->parent = this;
			this->children.insert(entity);
		}

		void Entity::removeChild(Entity* entity)
		{
			this->children.erase(entity);
			entity->parent = nullptr;
		}
	}
}


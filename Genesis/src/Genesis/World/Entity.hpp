#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Core/Transform.hpp"

#include <memory>

namespace Genesis
{
	//Prototype class
	class EntityManager;
	class World;
	class RootEntity;


	typedef uint32_t EntityId;
	const EntityId INVALID_ID = 0;

	enum class EntityType
	{
		GAMEOBJECT, //The root of an entity, holds the rigidbody
		CHILD, //Child of an entity
	};

	class Entity
	{
	public:
		const EntityId entityId;
		const EntityManager* manager;
		Entity(EntityManager* manager, EntityId id);
		virtual ~Entity();

		//World Functions
		virtual World* getWorld() { return this->world; };
		virtual void addToWorld(World* new_world);
		virtual void removeFromWorld();

		//Transform Functions
		virtual Transform getLocalTransform();
		virtual void setLocalTransform(Transform& transform);
		virtual void recalculateTransform() = 0;

		inline Transform getRelativeTransform() { return this->relative_transform; };
		inline Transform getWorldTransform() { return this->world_transform; };

		//Child Functions
		void addChild(Entity* child);
		void removeChild(Entity* child);

		virtual inline EntityType getNodeType() = 0;
	protected:

		//Transform Variables
		bool local_transform_changed = false;
		Transform local_transform;
		Transform relative_transform;
		Transform world_transform;

		Entity* parent_entity = nullptr;
		RootEntity* root_entity = nullptr;

		World* world = nullptr;

		vector<Entity*> child_entities;
	};
};
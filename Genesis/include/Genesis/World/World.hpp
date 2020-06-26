#pragma once

#include "Genesis/Physics/PhysicsWorld.hpp"

namespace Genesis
{
	typedef uint32_t WorldId;

	class Entity;
	class Node;

	class World
	{
	public:
		World(WorldId id);
		~World();

		inline WorldId getId() { return this->world_id; };

		void update(TimeStep time_step);

		inline PhysicsWorld* getPhysicsWorld() { return this->physics_world; };

		void addEntity(Entity* entity);
		void removeEntity(Entity* entity);
		inline const vector<Entity*>& getEntities() { return this->entities; };

		void setActiveCamera(Node* node) { this->active_camera = node; };
		Node* getActiveCamrea() { return this->active_camera; };

		TransformD getWorldOrgin();

		Node* castRay(vector3D start_position, vector3D end_pos);

	private:
		friend class Entity;

		const WorldId world_id;

		Entity* parent_entity = nullptr;

		PhysicsWorld* physics_world = nullptr;

		vector<Entity*> entities;

		Node* active_camera = nullptr;
	};
}
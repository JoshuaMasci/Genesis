#pragma once

#include "Genesis/Physics/PhysicsWorld.hpp"

namespace Genesis
{
	class Entity;
	class Node;

	class World
	{
	public:
		World();
		~World();

		void runSimulation(Application* application, TimeStep time_step);

		void addEntity(Entity* entity);
		void removeEntity(Entity* entity);
		inline const vector<Entity*>& getEntities() { return this->entities; };

		inline PhysicsWorld* getPhysicsWorld() { return this->physics_world; };

		void setActiveCamera(Node* node) { this->active_camera = node; };
		Node* getActiveCamrea() { return this->active_camera; };

	private:
		PhysicsWorld* physics_world = nullptr;

		vector<Entity*> entities;

		Node* active_camera = nullptr;
	};
}
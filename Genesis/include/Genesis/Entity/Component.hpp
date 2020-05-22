#pragma once

namespace Genesis
{
	class Entity;
	class World;

	class Component
	{
	public:
		Component(Entity* entity) { this->parent_entity = entity; };
		virtual ~Component() {};

		virtual void addtoWorld(World* world) = 0;
		virtual void removeFromWorld() = 0;

		virtual void onUpdate(TimeStep time_step) = 0;
		virtual void onTransformUpdate() = 0;

	protected:
		Entity* parent_entity;
	};
}
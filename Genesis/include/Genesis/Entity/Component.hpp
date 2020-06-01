#pragma once

namespace Genesis
{
	class Node;
	class World;

	class Component
	{
	public:
		Component(Node* parent) { this->parent = parent; };
		virtual ~Component() {};

		virtual void addtoWorld(World* world) = 0;
		virtual void removeFromWorld() = 0;

		virtual void onUpdate(TimeStep time_step) = 0;

	protected:
		Node* parent;
	};
}
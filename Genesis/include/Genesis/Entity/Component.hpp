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

		virtual void addtoWorld(World* world) {};
		virtual void removeFromWorld() {};

		virtual void onUpdate(TimeStep time_step) {};

	protected:
		Node* parent;
	};
}
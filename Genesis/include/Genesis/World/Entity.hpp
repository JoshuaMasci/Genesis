#pragma once

#include "Genesis/Core/TypeInfo.hpp"

#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/World/ComponentSet.hpp"
#include "Genesis/World/Hierarchy.hpp"

namespace Genesis
{
	class Entity
	{
	public:
		Entity(size_t id, const char* name) :id(id), name(name) {};
		const size_t id;
		NameComponent name;
		TransformD local_transform;
		ComponentSet components;
		Hierarchy herarchy;

		static void addChild(Entity* parent, Entity* child)
		{
			parent->herarchy.children.insert(child);
			child->herarchy.parent = parent;
		};

		static void removeChild(Entity* parent, Entity* child)
		{
			parent->herarchy.children.erase(child);
			child->herarchy.parent = nullptr;
		};
	};
}
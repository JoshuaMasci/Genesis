#include "Genesis/Component/Hierarchy.hpp"

namespace Genesis
{
	void Hierarchy::addChild(EntityRegisty& registry, EntityHandle parent, EntityHandle child)
	{
		GENESIS_ENGINE_ASSERT(registry.valid(parent), "Parent not valid");
		GENESIS_ENGINE_ASSERT(registry.valid(child), "Child not valid");
		GENESIS_ENGINE_ASSERT(registry.has<Hierarchy>(parent), "Parent not valid");
		GENESIS_ENGINE_ASSERT(registry.has<Hierarchy>(child), "Child not valid");

		Hierarchy& parent_hierarchy = registry.get<Hierarchy>(parent);
		Hierarchy& child_hierarchy = registry.get<Hierarchy>(child);

		child_hierarchy.parent = parent;

		if (parent_hierarchy.first == null_entity)
		{
			parent_hierarchy.first = child;
		}
		else
		{
			// get last children
			auto prev_entity = parent_hierarchy.first;
			auto current_hierarchy = registry.try_get<Hierarchy>(prev_entity);
			while (current_hierarchy != nullptr && current_hierarchy->next != null_entity)
			{
				prev_entity = current_hierarchy->next;
				current_hierarchy = registry.try_get<Hierarchy>(prev_entity);
			}
			// add new
			current_hierarchy->next = child;
			child_hierarchy.prev = prev_entity;
		}
	}

	void Hierarchy::removeChild(EntityRegisty& registry, EntityHandle parent, EntityHandle child)
	{
		GENESIS_ENGINE_ASSERT(registry.valid(parent), "Parent not valid");
		GENESIS_ENGINE_ASSERT(registry.valid(child), "Child not valid");
		GENESIS_ENGINE_ASSERT(registry.has<Hierarchy>(parent), "Parent not valid");
		GENESIS_ENGINE_ASSERT(registry.has<Hierarchy>(child), "Child not valid");

		Hierarchy& parent_hierarchy = registry.get<Hierarchy>(parent);
		Hierarchy& child_hierarchy = registry.get<Hierarchy>(child);

		if (child_hierarchy.prev == entt::null || !registry.valid(child_hierarchy.prev))
		{
			parent_hierarchy.first = child_hierarchy.next;

			if (child_hierarchy.next != null_entity)
			{
				auto next_hierarchy = registry.try_get<Hierarchy>(child_hierarchy.next);
				if (next_hierarchy != nullptr)
				{
					next_hierarchy->prev = null_entity;
				}
			}
		}
		else
		{
			auto prev_hierarchy = registry.try_get<Hierarchy>(child_hierarchy.prev);
			if (prev_hierarchy != nullptr)
			{
				prev_hierarchy->next = child_hierarchy.next;
			}
			if (child_hierarchy.next != null_entity)
			{
				auto next_hierarchy = registry.try_get<Hierarchy>(child_hierarchy.next);
				if (next_hierarchy != nullptr)
				{
					next_hierarchy->prev = child_hierarchy.prev;
				}
			}
		}

	}
}

#include "Genesis/Component/Hierarchy.hpp"

namespace Genesis
{
	void Hierarchy::addChild(EntityRegistry& registry, EntityHandle parent, EntityHandle child)
	{
		GENESIS_ENGINE_ASSERT(registry.valid(parent), "Parent not valid");
		GENESIS_ENGINE_ASSERT(registry.valid(child), "Child not valid");

		ParentNode& parent_node = registry.get_or_assign<ParentNode>(parent);
		ChildNode& child_node = registry.get_or_assign<ChildNode>(child);

		child_node.prev = null_entity;
		child_node.next = null_entity;
		child_node.parent = parent;

		if (parent_node.first == null_entity)
		{
			parent_node.first = child;
		}
		else
		{
			// get last child
			auto prev_entity = parent_node.first;
			auto current_hierarchy = registry.try_get<ChildNode>(prev_entity);
			while (current_hierarchy != nullptr && current_hierarchy->next != null_entity)
			{
				prev_entity = current_hierarchy->next;
				current_hierarchy = registry.try_get<ChildNode>(prev_entity);
			}
			// add new
			current_hierarchy->next = child;
			child_node.prev = prev_entity;
		}
	}

	void Hierarchy::removeChild(EntityRegistry& registry, EntityHandle parent, EntityHandle child)
	{
		GENESIS_ENGINE_ASSERT(registry.valid(parent), "Parent not valid");
		GENESIS_ENGINE_ASSERT(registry.valid(child), "Child not valid");
		GENESIS_ENGINE_ASSERT(registry.has<ParentNode>(parent), "Parent not valid");
		GENESIS_ENGINE_ASSERT(registry.has<ChildNode>(child), "Child not valid");

		ParentNode& parent_node = registry.get<ParentNode>(parent);
		ChildNode& child_node = registry.get<ChildNode>(child);

		if (child_node.prev == entt::null || !registry.valid(child_node.prev))
		{
			parent_node.first = child_node.next;

			if (child_node.next != null_entity)
			{
				auto next_hierarchy = registry.try_get<ChildNode>(child_node.next);
				if (next_hierarchy != nullptr)
				{
					next_hierarchy->prev = null_entity;
				}
			}
		}
		else
		{
			auto prev_hierarchy = registry.try_get<ChildNode>(child_node.prev);
			if (prev_hierarchy != nullptr)
			{
				prev_hierarchy->next = child_node.next;
			}
			if (child_node.next != null_entity)
			{
				auto next_hierarchy = registry.try_get<ChildNode>(child_node.next);
				if (next_hierarchy != nullptr)
				{
					next_hierarchy->prev = child_node.prev;
				}
			}
		}

		registry.remove<ChildNode>(child);
	}
}

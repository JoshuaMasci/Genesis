#include "Genesis/Scene/Hierarchy.hpp"

namespace Genesis
{
	void HierarchyUtils::addChild(EntityRegistry& registry, EntityHandle parent, EntityHandle child)
	{
		GENESIS_ENGINE_ASSERT(registry.valid(parent), "Parent not valid");
		GENESIS_ENGINE_ASSERT(registry.valid(child), "Child not valid");

		ParentNode& parent_node = registry.get_or_emplace<ParentNode>(parent);
		ChildNode& child_node = registry.get_or_emplace<ChildNode>(child);

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

	void HierarchyUtils::removeChild(EntityRegistry& registry, EntityHandle parent, EntityHandle child)
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

	HiearchyIterator::HiearchyIterator(EntityRegistry* registry, EntityHandle child)
	{
		this->registry = registry;
		this->child = child;
	}

	HiearchyIterator& HiearchyIterator::operator++()
	{
		if (this->registry->valid(this->child))
		{
			ChildNode* child_node = this->registry->try_get<ChildNode>(this->child);
			if (child_node != nullptr)
			{
				this->child = child_node->next;
			}
			else
			{
				this->child = null_entity;
			}
		}
		else
		{
			this->child = null_entity;
		}

		return *this;
	}

	HiearchyIterator HiearchyIterator::operator++(int)
	{
		HiearchyIterator iterator = *this;

		if (iterator.registry->valid(iterator.child))
		{
			ChildNode* child_node = iterator.registry->try_get<ChildNode>(iterator.child);
			if (child_node != nullptr)
			{
				iterator.child = child_node->next;
			}
			else
			{
				iterator.child = null_entity;
			}
		}
		else
		{
			iterator.child = null_entity;
		}

		return iterator;
	}

	bool HiearchyIterator::operator==(const HiearchyIterator& iterator)
	{
		return (this->registry == iterator.registry) && (this->child == iterator.child);
	}
	
	bool HiearchyIterator::operator!=(const HiearchyIterator& iterator)
	{
		return (this->registry != iterator.registry) || (this->child != iterator.child);
	}

	EntityHandle HiearchyIterator::operator*()
	{
		return this->child;
	}

	EntityHiearchy::EntityHiearchy(EntityRegistry* registry, EntityHandle parent)
	{
		this->registry = registry;
		this->parent = parent;
	}

	HiearchyIterator EntityHiearchy::begin()
	{
		if (this->registry && this->registry->valid(this->parent))
		{
			if (this->registry->has<ParentNode>(this->parent))
			{
				return HiearchyIterator(this->registry, this->registry->get<ParentNode>(this->parent).first);
			}
		}

		return HiearchyIterator(this->registry, null_entity);
	}

	HiearchyIterator EntityHiearchy::end()
	{
		return HiearchyIterator(this->registry, null_entity);
	}
}
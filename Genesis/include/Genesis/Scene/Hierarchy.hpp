#pragma once

#include "Genesis/Scene/Ecs.hpp"

namespace Genesis
{
	struct ParentNode
	{
		EntityHandle first{ null_entity };
	};

	struct ChildNode
	{
		EntityHandle prev{ null_entity };
		EntityHandle next{ null_entity };
		EntityHandle parent{ null_entity };
	};

	struct HierarchyUtils
	{
		static void addChild(EntityRegistry& registry, EntityHandle parent, EntityHandle child);
		static void removeChild(EntityRegistry& registry, EntityHandle parent, EntityHandle child);
	};

	class HiearchyIterator
	{
	protected:
		EntityRegistry* registry;
		EntityHandle child;

	public:
		HiearchyIterator(EntityRegistry* registry, EntityHandle child);

		HiearchyIterator& operator++();
		HiearchyIterator operator++(int);

		bool operator==(const HiearchyIterator& iterator);
		bool operator!=(const HiearchyIterator& iterator);
		EntityHandle operator*();
	};

	class EntityHiearchy
	{
	protected:
		EntityRegistry* registry;
		EntityHandle parent;

	public:
		EntityHiearchy(EntityRegistry* registry, EntityHandle parent);
		HiearchyIterator begin();
		HiearchyIterator end();
	};
}
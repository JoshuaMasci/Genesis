#pragma once

#include "Genesis/World/Entity.hpp"

namespace Genesis
{
	class ChildEntity : public Entity
	{
	public:
		ChildEntity(EntityManager* manager, EntityId id);

		virtual void recalculateTransform();

		virtual inline EntityType getNodeType() { return EntityType::CHILD; };
	protected:
	};
};
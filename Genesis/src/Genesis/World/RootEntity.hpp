#pragma once

#include "Genesis/World/Entity.hpp"

namespace Genesis
{
	class RootEntity : public Entity
	{
	public:
		RootEntity(EntityManager* manager, EntityId id);

		virtual void recalculateTransform();

		virtual inline EntityType getNodeType() { return EntityType::GAMEOBJECT; };
	protected:
	};
};
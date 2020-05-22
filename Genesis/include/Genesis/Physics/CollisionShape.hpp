#pragma once

#include "Genesis/Entity/Component.hpp"
#include "Genesis/Physics/ReactPhyscis.hpp"

namespace Genesis
{
	class CollisionShapeComponent : public Component
	{
	public:
		CollisionShapeComponent(Entity* entity, reactphysics3d::CollisionShape* shape);
		~CollisionShapeComponent();

		virtual void addtoWorld(World* world) override;
		virtual void removeFromWorld() override;
		virtual void onUpdate(TimeStep time_step) override;
		virtual void onTransformUpdate() override;

	private:
		reactphysics3d::CollisionShape* collision_shape = nullptr;
		reactphysics3d::ProxyShape* proxy_shape = nullptr;
	};

	class BoxShape : public CollisionShapeComponent
	{
	public:
		BoxShape(Entity* entity, vector3D size) : CollisionShapeComponent(entity, new reactphysics3d::BoxShape(toVec3R(size))) {};
	};
}

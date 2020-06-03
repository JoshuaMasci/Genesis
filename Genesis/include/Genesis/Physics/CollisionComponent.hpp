#pragma once

#include "Genesis/Physics/ReactPhyscis.hpp"

namespace Genesis
{
	class World;
	class Node;

	class CollisionComponent
	{
	public:
		CollisionComponent(Node* node, reactphysics3d::CollisionShape* shape);
		~CollisionComponent();

		void updateShape(reactphysics3d::CollisionShape* shape);
		void updateTransform(const TransformF& transform);

		void addtoWorld(World* world);
		void removeFromWorld();

	private:
		Node* parent = nullptr;
		reactphysics3d::CollisionShape* shape = nullptr;
		reactphysics3d::ProxyShape* proxy_shape = nullptr;
	};
}
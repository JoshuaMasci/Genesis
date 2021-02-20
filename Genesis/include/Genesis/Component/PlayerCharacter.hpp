#pragma once

#include "Genesis/Entity/Component.hpp"
#include "Genesis/Input/InputManager.hpp"

#include "Genesis/Entity/Entity.hpp"

namespace Genesis
{
	class PlayerCharacter : public Component
	{
	public:
		PlayerCharacter(Node* node, InputManager* input_manager)
			:Component(node)
		{
			this->input_manager = input_manager;
		};

		virtual void onUpdate(TimeStep time_step) override
		{
			if (this->parent->getRootEntity() != nullptr)
			{
				if (this->input_manager->getButtonPressed("Mouse_Left"))
				{
					TransformD world_transform = this->parent->getGlobalTransform();
					World* world = this->parent->getRootEntity()->getWorld();
					Entity* root = this->parent->getRootEntity();

					vector3D start_pos = world_transform.getPosition();
					vector3D end_pos = start_pos + (world_transform.getForward() * 10.0);

					Node* hit_node = world->castRay(start_pos, end_pos);

					if (hit_node != nullptr)
					{
						GENESIS_ENGINE_INFO("Hit");
					}
				}
			}
		};

	protected:
		InputManager* input_manager;
	};
}
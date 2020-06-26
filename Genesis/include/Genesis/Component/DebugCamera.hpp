#pragma once

#include "Genesis/Entity/Component.hpp"
#include "Genesis/Input/InputManager.hpp"

#include "Genesis/Entity/Entity.hpp"

namespace Genesis
{
	class DebugCamera : public Component
	{
	public:
		DebugCamera(Node* node, double linear, double angular, InputManager* input_manager)
			:Component(node)
		{
			this->linear_speed = linear;
			this->angular_speed = angular;
			this->input_manager = input_manager;
		};

		virtual void onUpdate(TimeStep time_step) override
		{
			if (this->parent->getRootEntity() != nullptr)
			{
				TransformD transform = this->parent->getRootEntity()->getWorldTransform();

				vector3D position = transform.getPosition();
				position += (transform.getForward() * input_manager->getButtonAxisCombo("Debug_ForwardBackward", "Debug_Forward", "Debug_Backward")) * this->linear_speed * time_step;
				position += (transform.getUp() * input_manager->getButtonAxisCombo("Debug_UpDown", "Debug_Up", "Debug_Down")) * this->linear_speed * time_step;
				position += (transform.getLeft() * input_manager->getButtonAxisCombo("Debug_LeftRight", "Debug_Left", "Debug_Right")) * this->linear_speed * time_step;
				transform.setPosition(position);

				quaternionD orientation = transform.getOrientation();
				orientation = glm::angleAxis(input_manager->getButtonAxisCombo("Debug_Pitch", "Debug_PitchUp", "Debug_PitchDown", false) * this->angular_speed * (PI_D * 2.0) * time_step, transform.getLeft()) * orientation;
				orientation = glm::angleAxis(input_manager->getButtonAxisCombo("Debug_Yaw", "Debug_YawLeft", "Debug_YawRight", false) * this->angular_speed * (PI_D * 2.0) * time_step, transform.getUp()) * orientation;
				orientation = glm::angleAxis(input_manager->getButtonAxisCombo("Debug_Roll", "Debug_RollRight", "Debug_RollLeft", false) * this->angular_speed * (PI_D)* time_step, transform.getForward()) * orientation;
				transform.setOrientation(orientation);

				this->parent->getRootEntity()->setWorldTransform(transform);
			}
		};

	protected:
		InputManager* input_manager;

		//meter per second
		double linear_speed;

		//rad per second
		double angular_speed;
	};
}
#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/WorldTransform.hpp"

#include "Genesis/Input/InputManager.hpp"

namespace Genesis
{
	struct DebugCamera
	{
		DebugCamera(double linear, double angular) : linear_speed(linear), angular_speed(angular) {};

		//meter per second
		double linear_speed;

		//rad per second
		double angular_speed;

		static void update(InputManager* input_manager, DebugCamera* debug_camera, WorldTransform* world_transform, double delta_time)
		{
			Transform& transform = world_transform->current_transform;

			vector3D position = transform.getPosition();
			position += (transform.getForward() * input_manager->getButtonAxisCombo("Debug_ForwardBackward", "Debug_Forward", "Debug_Backward")) * debug_camera->linear_speed * delta_time;
			position += (transform.getUp() * input_manager->getButtonAxisCombo("Debug_UpDown", "Debug_Up", "Debug_Down")) * debug_camera->linear_speed * delta_time;
			position += (transform.getLeft() * input_manager->getButtonAxisCombo("Debug_LeftRight", "Debug_Left", "Debug_Right")) * debug_camera->linear_speed * delta_time;
			transform.setPosition(position);

			quaternionD orientation = transform.getOrientation();
			orientation = glm::angleAxis(input_manager->getButtonAxisCombo("Debug_Pitch", "Debug_PitchUp", "Debug_PitchDown", false) * debug_camera->angular_speed * (PI * 2.0) * delta_time, transform.getLeft()) * orientation;
			orientation = glm::angleAxis(input_manager->getButtonAxisCombo("Debug_Yaw", "Debug_YawLeft", "Debug_YawRight", false) * debug_camera->angular_speed * (PI * 2.0) * delta_time, transform.getUp()) * orientation;
			orientation = glm::angleAxis(input_manager->getButtonAxisCombo("Debug_Roll", "Debug_RollRight", "Debug_RollLeft", false) * debug_camera->angular_speed * (PI) * delta_time, transform.getForward()) * orientation;
			transform.setOrientation(orientation);
		};
	};
}
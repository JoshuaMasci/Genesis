#pragma once

#include "Genesis/Ecs/EcsWorld.hpp"

namespace Genesis
{
	class InputEventSystem
	{
	public:
		virtual void on_button_press() = 0;
		virtual void on_axis_move() = 0;
	};

	class PreLogicUpdateSystem
	{
	public:
		virtual void pre_update(EcsWorld* world, TimeStep time_step) = 0;
	};

	class LogicUpdateSystem
	{
	public:
		virtual void update(EcsWorld* world, TimeStep time_step) = 0;
	};

	class RenderSystem
	{
	public:
		virtual void render(vector2U screen_size, EcsWorld* world, TimeStep time_step) = 0;
	};
}
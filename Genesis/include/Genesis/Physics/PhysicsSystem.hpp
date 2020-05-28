#pragma once

#include "Genesis/Ecs/EcsSystem.hpp"

namespace Genesis
{
	class PhyscisSystem : public LogicUpdateSystem
	{
	public:
		virtual void update(EcsWorld* world, TimeStep time_step) override;
	};
}

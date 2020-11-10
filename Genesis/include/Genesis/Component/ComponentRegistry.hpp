#pragma once

namespace Genesis
{
	class Entity;
	class World;

	typedef void(*OnEnabledFunction)(const TimeStep, World*, Entity*, const TransformD&, Entity*);
	typedef void(*onDisabledFunction)(const TimeStep, World*, Entity*, const TransformD&, Entity*);
}
#pragma once

#include <entt/entity/registry.hpp>

namespace Genesis
{
	typedef uint32_t EntityId;
	typedef entt::basic_registry<EntityId> EntityRegistry;
}
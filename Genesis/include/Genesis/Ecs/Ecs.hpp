#pragma once

#include "entt/entt.hpp"

namespace Genesis
{
	typedef entt::entity EntityHandle;
	typedef entt::basic_registry<EntityHandle> EntityRegistry;

	constexpr EntityHandle null_entity = entt::null;
}
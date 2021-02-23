#pragma once

#pragma warning( disable : 4307)
#include "entt/entt.hpp"

namespace genesis
{
	typedef entt::entity EntityHandle;
	typedef entt::basic_registry<EntityHandle> EntityRegistry;

	constexpr EntityHandle null_entity = entt::null;
}
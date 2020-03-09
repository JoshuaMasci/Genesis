#pragma once

//Ignore the C4307 on MVSC
#pragma warning(push)
#pragma warning(disable:4307)
#include <entt/entt.hpp>
#pragma warning(pop)

namespace Genesis
{
	typedef entt::entity EntityId;
	typedef entt::registry EntityRegistry;
} 
#pragma once

#include "entt/core/type_info.hpp"
#include "Genesis/Ecs/EntityWorld.hpp"
#include "Genesis/Component/TransformComponent.hpp"
namespace Genesis
{
	typedef void(*EntityOnEnabledFunction)(EntityWorld*, EntityHandle, const Transform&, EntityHandle);
	typedef void(*EntityOnDisabledFunction)(EntityWorld*, EntityHandle, EntityHandle);

	class ComponentEvents
	{
	protected:
		flat_hash_map<ENTT_ID_TYPE, EntityOnEnabledFunction> on_enabled_functions;
		flat_hash_map<ENTT_ID_TYPE, EntityOnDisabledFunction> on_disabled_functions;

	public:
		void add_enable_function(ENTT_ID_TYPE component_id, EntityOnEnabledFunction enabled_function) {};
		void add_disabled_function(ENTT_ID_TYPE component_id, EntityOnDisabledFunction disabled_function) {};

		void on_enabled(ENTT_ID_TYPE component_id, EntityWorld* world, EntityHandle root, const Transform& relative_transform, EntityHandle entity) {};
		void on_disabled(ENTT_ID_TYPE component_id, EntityWorld* world, EntityHandle root, const Transform& relative_transform, EntityHandle entity) {};
	};
}
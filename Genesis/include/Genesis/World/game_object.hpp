#pragma once

#include "Genesis/Core/VectorSet.hpp"

namespace Genesis
{
	typedef uint64_t GameObjectId;

	class GameObject;

	struct BehaviourEvent
	{
		size_t type_hash = 0;
		void* data = nullptr;
	};

	class Behaviour
	{
	protected:
		GameObject* parent = nullptr;

	public:
		virtual void on_enabled(GameObject* parent) = 0;
		virtual void on_disable() = 0;
		virtual void on_update(const double& time_step) = 0;
		virtual void on_event(const BehaviourEvent& behaviour_event) = 0;
	};

	class GameObject
	{
	protected:
		GameObjectId id;
		bool enabled = false;

		TransformD local_transform;

		GameObject* root;
		GameObject* parent;
		vector_set<GameObject*> children;

	public:
		GameObject();
		~GameObject();

		void enable();
		void disable();

		void update(const double& time_step);

		template<typename T, typename... Args>
		void emit_event(Args&&... args)
		{
			T temp_event(std::forward<Args>(args)...);
			BehaviourEvent behaviour_event = { typeid(T).hash_code, (void*)&temp_event };
		};

		void add_child(GameObject* child);
		void remove_child(GameObject* child);
	};
}
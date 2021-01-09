#pragma once

#include "Genesis/Core/VectorSet.hpp"
#include "Genesis/World/Behaviour.hpp"
#include "Genesis/Component/NameComponent.hpp"

namespace Genesis
{
	class GameWorld;

	typedef flat_hash_map<size_t, Behaviour*> BehaviourMap;

	class GameObject
	{
		friend class GameWorld;

	protected:
		string name;

		GameWorld* world = nullptr;

		bool transform_changed = false;
		TransformD local_transform;
		TransformD root_transform;

		GameObject* root = nullptr;
		GameObject* parent = nullptr;
		vector_set<GameObject*> children;

		BehaviourMap behaviour_map;

	public:
		GameObject(const string& name = "No Name");
		~GameObject();

		string& get_name() { return this->name; };
		void set_name(const string& name) { this->name = name; };
		GameWorld* get_world() { return this->world; };
		GameObject* get_root() { return this->root; };
		GameObject* get_parent() { return this->parent; };

		TransformD get_local_transform() const { return this->local_transform; };
		void set_local_transform(const TransformD& transform) 
		{
			this->local_transform = transform;
			this->transform_changed = true;
		};

		void add_to_world(GameWorld* world);
		void remove_from_world();

		void update(const TimeStep& time_step);

		template<typename T, typename... Args>
		void emit_event(Args&&... args)
		{
			T temp_event(std::forward<Args>(args)...);
			BehaviourEvent behaviour_event = { typeid(T).hash_code, (void*)&temp_event };
		};

		void add_child(GameObject* child);
		void remove_child(GameObject* child);
		vector_set<GameObject*>& get_children() { return this->children; };

		bool has(size_t type_id);
		Behaviour* get(size_t type_id);
		void remove(size_t type_id);

		template<typename T, typename... TArgs> T* add(TArgs&&... mArgs)
		{
			if (!this->hasComponent<T>())
			{
				Behaviour* behaviour = new T(this, std::forward<TArgs>(mArgs)...);
				this->behaviour_map[typeid(T).hash_code()] = behaviour;

				if (this->world)
				{
					behaviour->on_enabled(this);
				}

				return (T*)behaviour;
			}
			else
			{
				GENESIS_ENGINE_ERROR("{}:{} already has {}", this->id, this->name, typeid(T).name());
				return nullptr;
			}
		};

		template<typename T> bool has()
		{
			return this->behaviour_map.find(typeid(T).hash_code()) != this->behaviour_map.end();
		};

		template<typename T> T* get()
		{
			if (this->hasComponent<T>())
			{
				return (T*)this->behaviour_map[typeid(T).hash_code()];
			}
			return nullptr;
		};

		template<typename T> void remove()
		{
			this->remove(typeid(T).hash_code());
		};

		BehaviourMap& get_behaviours() { return this->behaviour_map; };
	};
}
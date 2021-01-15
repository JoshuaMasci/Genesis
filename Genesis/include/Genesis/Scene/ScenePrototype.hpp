#pragma once

#include "Genesis/Core/VectorSet.hpp"

namespace Genesis
{
	class ComponentMapPrototype
	{
	protected:
		flat_hash_map<size_t, void*> component_map;

	public:
		virtual ~ComponentMapPrototype()
		{
			for (auto component : this->component_map)
			{
				delete component.second;
			}
		}

		template<typename T, typename... TArgs> T* add_component(TArgs&&... mArgs)
		{
			if (!this->has_component<T>())
			{
				void* component = new T(std::forward<TArgs>(mArgs)...);
				this->component_map[typeid(T).hash_code()] = component;
				return (T*)component;
			}
			else
			{
				return nullptr;
			}
		};

		template<typename T> bool has_component()
		{
			return this->component_map.find(typeid(T).hash_code()) != this->component_map.end();
		};

		template<typename T> T* get_component()
		{
			auto it = this->component_map.find(typeid(T).hash_code());
			if (it != this->component_map.end())
			{
				return (T*)it->second;
			}
			return nullptr;
		};

		template<typename T> void remove_component()
		{
			this->component_map.erase(typeid(T).hash_code());
		};
	};

	class EntityPrototype;
	class ScenePrototype;

	typedef vector_set<EntityPrototype*> EntitySet;

	class EntityPrototype: public ComponentMapPrototype
	{
	protected:
		string name;
		TransformD local_transform;

		ScenePrototype* scene = nullptr;
		EntityPrototype* parent = nullptr;
		EntitySet children;

		friend class ScenePrototype;

	public:
		EntityPrototype(const string& entity_name): name(entity_name) {};

		virtual ~EntityPrototype()
		{
			for (auto entity : this->children)
			{
				delete entity;
			}
		}

		string get_name() { return this->name; };
		void set_name(const string& name) { this->name = name; };

		TransformD& get_transform() { return this->local_transform; };
		void set_transform(const TransformD& transform) { this->local_transform = transform; };

		ScenePrototype* get_scene() { return this->scene; };
		EntityPrototype* get_parent() { return this->parent; };

		void add_child(EntityPrototype* child) { child->scene = nullptr; child->parent = this; this->children.insert(child); };
		void remove_child(EntityPrototype* child) { child->scene = nullptr; child->parent = nullptr; this->children.erase(child); };
		EntitySet& get_children() { return this->children; };
	};

	class ScenePrototype: public ComponentMapPrototype
	{
	protected:
		EntitySet entities;

	public:
		void add_entity(EntityPrototype* entity) { entity->scene = this; entity->parent = nullptr; this->entities.insert(entity); };
		void remove_entity(EntityPrototype* entity) { entity->scene = nullptr; entity->parent = nullptr; this->entities.erase(entity); };
		EntitySet& get_entities() { return this->entities; };

	};
}
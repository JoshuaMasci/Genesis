#pragma once

#include "Genesis/Entity/Component.hpp"

namespace Genesis
{
	//Prototype Defintions;
	class World;
	class RigidBody;
	class CollisionShape;

	typedef uint64_t EntityId;
	const EntityId InvalidEntity = 0;

	class Entity
	{
		friend class World;
	public:
		Entity(EntityId id);
		Entity(EntityId id, string name);
		~Entity();

		const inline EntityId getId() { return this->id; };
		const inline string getName() { return this->name; };
		void setName(const string& new_name) { this->name = new_name; };

		TransformD getWorldTransform();
		void setTransform(const TransformD& transform);

		inline World* getWorld() { return this->world; };

		void createRigidbody();
		void removeRigidbody();

		bool hasRigidbody() { return this->rigidbody != nullptr; };
		RigidBody* getRigidbody() { return this->rigidbody; };

	protected:
		//Entity Info
		EntityId id = InvalidEntity;
		string name;

		//Transform
		TransformD world_transform;

		//Hierarchy Info 
		World* world = nullptr;

		void addtoWorld(World* world);
		void removeFromWorld();

		RigidBody* rigidbody = nullptr;
	};

	class Node
	{
		friend class Entity;
	public:

		inline TransformD getLocalTransform() { return this->local_transform; };
		inline TransformD getRootTransform() { return this->root_transform; };
		inline TransformD getGlobalTransform() { return this->root_transform; };//TODO

		void addChild(Node* child);
		void removeChild(Node* child);
		inline const vector<Node*>& getChildren() { return this->children; };

		template<typename T, typename... TArgs> T* addComponent(TArgs&&... mArgs)
		{
			if (!this->hasComponent<T>())
			{
				Component* component = new T(this, std::forward<TArgs>(mArgs)...);
				this->component_map[typeid(T).hash_code()] = component;

				if (this->world != nullptr)
				{
					component->addtoWorld(this->world);
				}

				return (T*)component;
			}
			else
			{
				GENESIS_ENGINE_ERROR("{}:{} already has {}", this->id, this->name, typeid(T).name());
				return nullptr;
			}
		};

		template<typename T> bool hasComponent()
		{
			return this->component_map.find(typeid(T).hash_code()) != this->component_map.end();
		};

		template<typename T> T* getComponent()
		{
			if (this->hasComponent<T>())
			{
				return (T*)this->component_map[typeid(T).hash_code()];
			}
			return nullptr;
		};

		template<typename T> void removeComponent()
		{
			if (this->hasComponent<T>())
			{
				size_t type = typeid(T).hash_code();

				if (this->world != nullptr)
				{
					this->component_map[type]->removeFromWorld();
				}
				delete this->component_map[type];
				this->component_map.erase(type);
			}
		};

	protected:
		string name;

		TransformD local_transform;
		TransformD root_transform;//relative to root

		Entity* root = nullptr;

		Node* parent = nullptr;
		vector<Node*> children;

		//Components
		map<size_t, Component*> component_map;
	};

}
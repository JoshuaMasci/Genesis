#pragma once

#include "Genesis/Entity/Component.hpp"

namespace Genesis
{
	//Prototype Defintions;
	class World;
	class RigidBody;
	class CollisionComponent;

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
		void setWorldTransform(const TransformD& transform);

		inline World* getWorld() { return this->world; };
		inline Node* getRootNode() { return this->root_node; };

		void createRigidbody();
		void removeRigidbody();

		bool hasRigidbody() { return this->rigidbody != nullptr; };
		RigidBody* getRigidbody() { return this->rigidbody; };

		void onUpdate(TimeStep time_step);

	protected:
		void addtoWorld(World* world);
		void removeFromWorld();

		//Entity Info
		EntityId id = InvalidEntity;
		string name;

		//Transform
		TransformD world_transform;

		//Hierarchy Info 
		World* world = nullptr;
		Node* root_node = nullptr;

		RigidBody* rigidbody = nullptr;
	};

	class Node
	{
		friend class Entity;
	public:
		Node(string name = "");
		~Node();

		const inline string getName() { return this->name; };
		void setName(const string& new_name) { this->name = new_name; };

		void setLocalTransform(const TransformF& transform);
		inline TransformF getLocalTransform() { return this->local_transform; };
		inline TransformF getRootTransform() { return this->root_transform; };
		TransformD getGlobalTransform();

		inline Entity* getRootEntity() { return this->root; };

		void addChild(Node* child);
		void removeChild(Node* child);
		inline const vector<Node*>& getChildren() { return this->children; };

		void createCollisionShape();
		void removeCollisionShape();
		inline CollisionComponent* getCollisionShape() { return this->collision_shape; };

		template<typename T, typename... TArgs> T* addComponent(TArgs&&... mArgs)
		{
			if (!this->hasComponent<T>())
			{
				Component* component = new T(this, std::forward<TArgs>(mArgs)...);
				this->component_map[typeid(T).hash_code()] = component;

				if (this->root != nullptr && this->root->getWorld() != nullptr)
				{
					component->addtoWorld(this->root->getWorld());
				}

				return (T*)component;
			}
			else
			{
				GENESIS_ENGINE_ERROR("{} already has {}", this->name, typeid(T).name());
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

				if (this->root != nullptr && this->root->getWorld() != nullptr)
				{
					this->component_map[type]->removeFromWorld();
				}
				delete this->component_map[type];
				this->component_map.erase(type);
			}
		};

		void onUpdate(TimeStep time_step);

	protected:
		void addtoWorld(World* world);
		void removeFromWorld();
		void updateTransform();

		void addToEntity();
		void removeFromEntity();

		string name;

		TransformF local_transform;
		TransformF root_transform;//relative to root

		Entity* root = nullptr;

		Node* parent = nullptr;
		vector<Node*> children;

		//Components
		CollisionComponent* collision_shape;
		map<size_t, Component*> component_map;
	};

}
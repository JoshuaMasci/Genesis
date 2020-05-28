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

		enum class TransformDirtyFlag
		{
			None = 0,
			Global = 1, //Just update global transform
			All = 2 //Update global and root transforms
		};

	public:
		Entity(EntityId id);
		Entity(EntityId id, string name);
		~Entity();

		const inline EntityId getId() { return this->id; };
		const inline string getName() { return this->name; };
		void setName(const string& new_name) { this->name = new_name; };

		void updateTransform(TransformDirtyFlag parent_flag = TransformDirtyFlag::None);
		void onTransformUpdate(TransformDirtyFlag transform_dirty);

		inline void setLocalTransform(const TransformD& transform) { this->local_transform = transform; this->transform_dirty = true; };
		inline void setLocalPosition(const vector3D& position) { this->local_transform.setPosition(position); this->transform_dirty = true; };
		inline void setLocalOrientation(const quaternionD& orientation) { this->local_transform.setOrientation(orientation); this->transform_dirty = true; };
		inline void setLocalScale(const vector3D& scale) { this->local_transform.setScale(scale); this->transform_dirty = true; };

		inline TransformD getLocalTransform() { return this->local_transform; };
		inline TransformD getRootTransform() { return this->root_transform; };
		inline TransformD getGlobalTransform() { return this->global_transform; };

		inline World* getWorld() { return this->world; };
		inline Entity* getRoot() { return this->root; };
		inline bool isRoot() { return this == this->root; };
		inline Entity* getParent() { return this->parent; };

		void addChild(Entity* child);
		void removeChild(Entity* child);
		inline const vector<Entity*>& getChildren() { return this->children; };

		void createRigidbody();
		void removeRigidbody();

		bool hasRigidbody() { return this->rigidbody != nullptr; };
		RigidBody* getRigidbody() { return this->rigidbody; };

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
		//Entity Info
		EntityId id = InvalidEntity;
		string name;

		//Transform
		TransformD local_transform;
		TransformD root_transform;//relative to root
		TransformD global_transform;
		bool transform_dirty = true;

		//Hierarchy Info 
		World* world = nullptr;
		Entity* root = nullptr;
		Entity* parent = nullptr;
		vector<Entity*> children;

		void addtoWorld(World* world);
		void removeFromWorld();

		//Fixed Components
		RigidBody* rigidbody = nullptr;

		//Components
		map<size_t, Component*> component_map;
	};
}
#pragma once

namespace Genesis
{
	class World;

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

		void updateTransform(bool has_parent_changed = true);

		inline void setLocalTransform(const TransformD& transform) { this->local_transform = transform; this->local_transform_dirty = true; };
		inline void setLocalPosition(const vector3D& position) { this->local_transform.setPosition(position); this->local_transform_dirty = true; };
		inline void setLocalOrientation(const quaternionD& orientation) { this->local_transform.setOrientation(orientation); this->local_transform_dirty = true; };
		inline void setLocalScale(const vector3D& scale) { this->local_transform.setScale(scale); this->local_transform_dirty = true; };

		inline TransformD getLocalTransform() { return this->local_transform; };
		inline TransformD getGlobalTransform() { return this->global_transform; };

		inline World* getWorld() { return this->world; };
		inline Entity* getRoot() { return this->root; };
		inline bool isRoot() { return this == this->root; };
		inline Entity* getParent() { return this->parent; };

		void addChild(Entity* child);
		void removeChild(Entity* child);
		inline const vector<Entity*>& getChildren() { return this->children; };

	protected:
		//Entity Info
		EntityId id = InvalidEntity;
		string name;

		//Transform
		TransformD local_transform;
		bool local_transform_dirty = true;
		TransformD global_transform;

		//Hierarchy Info 
		World* world = nullptr;
		Entity* root = nullptr;
		Entity* parent = nullptr;
		vector<Entity*> children;
		void addtoWorld(World* world);
		void removeFromWorld();

		//Component Info
	};
}
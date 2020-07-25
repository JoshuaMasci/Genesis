#pragma once

#include "Genesis/ECS/EntityTypes.hpp"
#include "Genesis/ECS/BlockArray.hpp"

namespace Genesis
{
	class EntityWorld;

	struct ComponentRegistryInfo
	{
		flat_hash_map<size_t, ComponentId> component_hash_to_id_map;
		vector<ComponentInfo> component_info;

		template<class Component>
		ComponentId getComponentID()
		{
			size_t hash_value = TypeInfo<Component>().getHash();
			GENESIS_ENGINE_ASSERT(has_value(this->component_hash_to_id_map, hash_value), "Component Not Registered");
			return this->component_hash_to_id_map[hash_value];
		};
	};

	struct EntityRegistryInfo
	{
		EntityId next_entity_id = 0;
		flat_hash_map<EntityId, EntityWorld*> entity_world_map;
	};

	class ComponentPool
	{
	protected:
		const size_t GROWTH_RATE = 2;
		const size_t MIN_BLOCK_COUNT = 16;

		const ComponentInfo component_info;

		vector<EntityId> entities_index_to_handle;
		flat_hash_map<EntityId, size_t> entities_handle_to_index;

		BlockArray memory_block;

	public:
		ComponentPool(const ComponentInfo& component)
			:component_info(component), memory_block(component.component_size, this->MIN_BLOCK_COUNT)
		{
			
		};

		~ComponentPool()
		{
			for (size_t i = 0; i < this->entities_index_to_handle.size(); i++)
			{
				this->component_info.component_delete_function(this->memory_block.getBlock(i));
			}
		};

		void* addEntity(EntityId entity)
		{
			GENESIS_ENGINE_ASSERT(!(has_value(this->entities_handle_to_index, entity)), ("Entity already has component:{}", this->component_info.component_id));
			//Fill the Entity Tables
			size_t new_entity_index = this->entities_index_to_handle.size();
			this->entities_index_to_handle.push_back(entity);
			this->entities_handle_to_index[entity] = new_entity_index;

			//Check if memory needs resizing
			size_t current_block_count = this->memory_block.getBlockCount();
			if (new_entity_index >= current_block_count)
			{
				this->memory_block.resize(current_block_count * this->GROWTH_RATE);
			}

			return this->memory_block.getBlock(new_entity_index);
		}

		void removeEntity(EntityId entity)
		{
			GENESIS_ENGINE_ASSERT(has_value(this->entities_handle_to_index, entity), ("Entity Doesn't have component:{}", this->component_info.component_id));
			size_t index_to_remove = this->entities_handle_to_index[entity];
			size_t index_last = this->entities_index_to_handle.size() - 1;

			//If entity is not the last one
			//Swap the last entity into it's slot
			if (index_to_remove != index_last)
			{
				this->memory_block.moveBlock(index_last, index_to_remove);
				this->entities_index_to_handle[index_to_remove] = this->entities_index_to_handle[index_last];
				this->entities_handle_to_index[this->entities_index_to_handle[index_to_remove]] = index_to_remove;
			}

			//Remove Last
			this->entities_index_to_handle.pop_back();
			this->entities_handle_to_index.erase(entity);

			//Check if table can be shrunk
			size_t current_entity_count = this->entities_index_to_handle.size();
			size_t current_block_count = this->memory_block.getBlockCount();
			if (current_block_count > (current_entity_count * GROWTH_RATE))
			{
				size_t new_size = std::max(current_block_count / 2, MIN_BLOCK_COUNT);
				this->memory_block.resize(new_size);
			}
		}

		void destroyEntity(EntityId entity)
		{
			GENESIS_ENGINE_ASSERT(has_value(this->entities_handle_to_index, entity), ("Entity Doesn't have component:{}", this->component_info.component_id));

			this->component_info.component_delete_function(this->memory_block.getBlock(this->entities_handle_to_index[entity]));
		}

		bool hasEntity(EntityId entity)
		{
			return has_value(this->entities_handle_to_index, entity);
		}

		size_t getSize()
		{
			return this->entities_index_to_handle.size();
		}

		size_t getCapacity()
		{
			return this->memory_block.getBlockCount();
		}

		void* getComponent(EntityId entity)
		{
			return this->memory_block.getBlock(this->entities_handle_to_index[entity]);
		}

		EntityId getEntityHandle(size_t index)
		{
			return this->entities_index_to_handle[index];
		}

		void* getComponentIndex(size_t index)
		{
			return this->memory_block.getBlock(index);
		}
	};

	class SingleComponentView
	{
	protected:
		ComponentPool* component_pool = nullptr;

	public:
		SingleComponentView(ComponentPool* pool) :component_pool(pool) {};

		size_t getSize()
		{
			return component_pool != nullptr ? this->component_pool->getSize() : 0;
		}

		EntityId getEntity(size_t index)
		{
			return component_pool->getEntityHandle(index);
		}

		void* getComponent(size_t index)
		{
			return component_pool->getComponentIndex(index);
		}
	};

	class MultiComponentView
	{
	protected:
		vector<ComponentPool*> component_pools;
		size_t primary_pool;

	public:
		MultiComponentView(vector<ComponentPool*>& pools)
		{
			this->component_pools = pools;

			size_t smallest_pool_index = 0;
			size_t smallest_pool_size = this->component_pools[0]->getSize();

			for (size_t i = 1; i < this->component_pools.size(); i++)
			{
				if (this->component_pools[i]->getSize() < smallest_pool_index)
				{
					smallest_pool_index = i;
					smallest_pool_size = this->component_pools[i]->getSize();
				}
			}

			this->primary_pool = smallest_pool_index;
		}

		size_t getSize()
		{
			return this->component_pools[this->primary_pool]->getSize();
		}

		EntityId getEntity(size_t entity_index)
		{
			return this->component_pools[this->primary_pool]->getEntityHandle(entity_index);
		}

		void* getComponent(size_t entity_index, size_t pool_index)
		{
			if (pool_index == this->primary_pool)
			{
				return this->component_pools[pool_index]->getComponentIndex(entity_index);
			}
			else
			{
				return this->component_pools[pool_index]->getComponent(this->component_pools[this->primary_pool]->getEntityHandle(entity_index));
			}
		}
	};

	class EntityWorld
	{
	protected:
		friend class EntityRegistry;

		//Registry Info
		ComponentRegistryInfo* component_registry_info;
		EntityRegistryInfo* entity_registry_info;

		vector<ComponentPool*> component_pools;

		flat_hash_set<EntityId> entities; // Entities in the world, might switch to track the components that the entity has but not for now

		bool hasComponentPool(ComponentId component_id)
		{
			if (component_id >= this->component_pools.size())
			{
				return false;
			}

			return this->component_pools[component_id] != nullptr;
		}

		ComponentPool* getComponentPool(ComponentId component_id)
		{
			if (component_id >= this->component_pools.size())
			{
				return nullptr;
			}

			return this->component_pools[component_id];
		}

		void createComponentPool(ComponentId component_id)
		{
			if (component_id >= this->component_pools.size())
			{
				size_t old_size = this->component_pools.size();
				size_t new_size = component_id + 1;
				this->component_pools.resize(component_id + 1);

				for (size_t i = old_size; i < new_size; i++)
				{
					this->component_pools[i] = nullptr;
				}
			}

			if (this->component_pools[component_id] == nullptr)
			{
				this->component_pools[component_id] = new ComponentPool(this->component_registry_info->component_info[component_id]);
			}
		}

		// Adds the entity to the world
		void addEntity(EntityId entity)
		{
			GENESIS_ENGINE_ASSERT(!has_value(this->entities, entity), "Entity already exist in this world");
			this->entities.insert(entity);
		}

		// Removes the entity from the world
		void removeEntity(EntityId entity)
		{
			GENESIS_ENGINE_ASSERT(has_value(this->entities, entity), "Entity doesn't exist in this world");

			for (size_t i = 0; i < this->component_pools.size(); i++)
			{
				if (this->component_pools[i] != nullptr)
				{
					this->component_pools[i]->removeEntity(entity);
				}
			}

			this->entities.erase(entity);
		}

		// Deconstructs the entity and removes it from the world
		void deleteEntity(EntityId entity)
		{
			GENESIS_ENGINE_ASSERT(has_value(this->entities, entity), "Entity doesn't exist in this world");

			for (size_t i = 0; i < this->component_pools.size(); i++)
			{
				if (this->component_pools[i] != nullptr)
				{
					this->component_pools[i]->destroyEntity(entity);
					this->component_pools[i]->removeEntity(entity);
				}
			}

			this->entities.erase(entity);
		}

	public:
		EntityWorld(ComponentRegistryInfo* component_registry_info, EntityRegistryInfo* entity_registry_info) { this->component_registry_info = component_registry_info; this->entity_registry_info = entity_registry_info; };

		~EntityWorld()
		{
			for (size_t i = 0; i < this->component_pools.size(); i++)
			{
				delete this->component_pools[i];
			}
		}

		EntityId createEntity()
		{
			EntityId new_entity = this->entity_registry_info->next_entity_id++;
			this->addEntity(new_entity);
			this->entity_registry_info->entity_world_map[new_entity] = this;
			return new_entity;
		}

		void destroyEntity(EntityId entity)
		{
			this->deleteEntity(entity);
			this->entity_registry_info->entity_world_map.erase(entity);
		}

		template<class Component>
		bool hasComponent(EntityId entity)
		{
			return this->hasComponent(this->component_registry_info->getComponentID<Component>(), entity);
		}

		bool hasComponent(ComponentId component, EntityId entity)
		{
			if (this->hasComponentPool(component))
			{
				if (this->getComponentPool(component)->hasEntity(entity))
				{
					return true;
				}
			}

			return false;
		}

		template<class Component>
		Component* getComponent(EntityId entity)
		{
			return (Component*)this->getComponent(this->component_registry_info->getComponentID<Component>(), entity);
		}

		void* getComponent(ComponentId component, EntityId entity)
		{
			if (this->hasComponentPool(component))
			{
				return this->getComponentPool(component)->getComponent(entity);
			}

			return nullptr;
		}

		template<class Component, typename... Args>
		Component* addComponent(EntityId entity, Args &&... args)
		{
			Component* component_ptr = (Component*)this->addComponent(this->component_registry_info->getComponentID<Component>(), entity);
			return new(component_ptr)Component(args...);
		}

		void* addComponent(ComponentId component, EntityId entity)
		{
			if (!this->hasComponentPool(component))
			{
				this->createComponentPool(component);
			}

			return this->getComponentPool(component)->addEntity(entity);
		}

		template<class Component>
		void removeComponent(EntityId entity)
		{
			this->removeComponent(this->component_registry_info->getComponentID<Component>(), entity)
		}

		void removeComponent(ComponentId component, EntityId entity)
		{
			if (this->hasComponentPool(component))
			{
				ComponentPool* pool = this->getComponentPool(component);
				pool->destroyEntity(entity);
				pool->removeEntity(entity);
			}
		}

		flat_hash_set<EntityId>& getAllEntities()
		{
			return this->entities;
		}

		SingleComponentView getView(ComponentId component_id)
		{
			if (this->hasComponentPool(component_id))
			{
				return SingleComponentView(this->component_pools[component_id]);
			}
			else
			{
				return SingleComponentView(nullptr);
			}
		}


		MultiComponentView getMultiComponentView(const vector<ComponentId>& components)
		{
			vector<ComponentPool*> pools(components.size());

			for (size_t i = 0; i < pools.size(); i++)
			{
				pools[i] = this->getComponentPool(components[i]);
			}

			return MultiComponentView(pools);
		}

	};

	class EntityRegistry
	{
	protected:
		ComponentRegistryInfo component_registry_info;
		EntityRegistryInfo entity_registry_info;
		flat_hash_set<EntityWorld*> worlds;

	public:
		EntityRegistry()
		{
		
		}

		~EntityRegistry()
		{
			for (EntityWorld* world : this->worlds)
			{
				delete world;
			}
		}

		//COMPONENT FUNCTIONS
		ComponentId registerComponent(size_t hash, size_t size, ComponentDeleteFunction deconstructor)
		{
			GENESIS_ENGINE_ASSERT(!has_value(this->component_registry_info.component_hash_to_id_map, hash), "Component Registered Already");

			ComponentId component_id = (ComponentId)this->component_registry_info.component_info.size();
			this->component_registry_info.component_hash_to_id_map[hash] = component_id;
			this->component_registry_info.component_info.push_back({ component_id, size, deconstructor });

			return component_id;
		}

		template<class Component>
		ComponentId registerComponent()
		{
			return this->registerComponent(TypeInfo<Component>().getHash(), TypeInfo<Component>().getSize(), [](void* ptr) { ((Component*)ptr)->~Component(); });
		}
		
		template<class Component>
		ComponentId getComponentID()
		{
			return this->component_registry_info.getComponentID<Component>();
		}

		//World Functions
		EntityWorld* createWorld()
		{
			EntityWorld* new_world = new EntityWorld(&this->component_registry_info, &this->entity_registry_info);
			this->worlds.insert(new_world);
			return new_world;
		}

		void destroyWorld(EntityWorld* world)
		{
			GENESIS_ENGINE_ASSERT(has_value(this->worlds, world), "World is not part of this Registry");
			this->worlds.erase(world);
			delete world;
		}

		void moveEntity(EntityId entity, EntityWorld* source_world, EntityWorld* destination_world)
		{

		}

		EntityRegistry* clone()
		{
			return nullptr;
		}

		void clear()
		{

		}
	};
}
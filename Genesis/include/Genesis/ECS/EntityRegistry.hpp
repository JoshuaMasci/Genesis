#pragma once

#include "Genesis/ECS/EntityTypes.hpp"
#include "Genesis/ECS/EntityPool.hpp"

namespace Genesis
{
	class EntityWorld;

	struct ComponentRegistryInfo
	{
		ComponentId next_component_id = 0;
		ComponentInfo component_info[MAX_COMPONENTS];
		flat_hash_map<size_t, ComponentId> component_hash_to_id_map;

		template<class Component>
		ComponentId getComponentID()
		{
			size_t hash_value = TypeInfo<Component>().getHash();
			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->component_hash_to_id_map, hash_value), "Component Not Registered");
			return this->component_hash_to_id_map[hash_value];
		};

		template<class Component>
		EntitySignature getSignature()
		{
			EntitySignature signature(0);
			signature[this->getComponentID<Component>()] = true;
			return signature;
		}

		template<class Component1, class Component2, class... Components>
		EntitySignature getSignature()
		{
			return this->getSignature<Component1>() | this->getSignature<Component2, Components...>();
		}
	};

	struct EntityRegistryInfo
	{
		EntityId next_entity_id = 0;
		flat_hash_map<EntityId, EntityWorld*> entity_world_map;
	};

	class EntityWorld
	{
	public:

		flat_hash_map<EntityId, EntitySignature>& getAllEntities()
		{
			return this->entity_map;
		}

		//Entity Functions
		template<class... Components>
		EntityId createEntity() { return this->createEntity(this->component_registry_info->getSignature<Components...>()); };
		EntityId createEntity(EntitySignature entity_signature)
		{
			EntityId new_entity = this->entity_registry_info->next_entity_id++;
			this->addEntity(new_entity, entity_signature);
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
			ComponentId component_id = this->component_registry_info->getComponentID<Component>();
			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->entity_map, entity), "Entity doesn't exist");
			EntitySignature entity_signature = this->entity_map[entity];
			return entity_signature[component_id];
		}

		template<class Component>
		Component* getComponent(EntityId entity)
		{
			ComponentId component_id = this->component_registry_info->getComponentID<Component>();

			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->entity_map, entity), "Entity doesn't exist");
			EntitySignature entity_signature = this->entity_map[entity];

			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->entity_pool_map, entity_signature), "Entity Pool doesn't exist");
			return (Component*)this->entity_pool_map[entity_signature]->getComponent(entity, component_id);
		};

		template<class Component, typename... Args>
		Component* initalizeComponent(EntityId entity, Args &&... args)
		{
			ComponentId component_id = this->component_registry_info->getComponentID<Component>();

			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->entity_map, entity), "Entity doesn't exist");
			EntitySignature entity_signature = this->entity_map[entity];

			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->entity_pool_map, entity_signature), "Entity Pool doesn't exist");
			void* component_ptr = this->entity_pool_map[entity_signature]->getComponent(entity, component_id);
			return new(component_ptr)Component(args...);
		};

		template<class Component, typename... Args>
		Component* addComponent(EntityId entity, Args &&... args)
		{
			ComponentId new_component_id = this->component_registry_info->getComponentID<Component>();
			EntitySignature component_sig(0);
			component_sig[new_component_id] = true;

			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->entity_map, entity), "Entity doesn't exist");
			EntitySignature entity_signature = this->entity_map[entity];
			EntityPool* source_pool = this->getEntityPool(entity_signature);

			//Check if the entity already has the component
			if ((entity_signature & component_sig).none())
			{
				EntitySignature new_entity_signature = entity_signature | component_sig;
				EntityPool* destination_pool = this->getEntityPool(new_entity_signature);
				destination_pool->addEntity(entity);
				
				//Copy all old components
				vector<EntityPool::PoolComponentInfo>& source_pool_components = source_pool->getComponentInfo();
				for (size_t i = 0; i < source_pool_components.size(); i++)
				{
					EntityPool::PoolComponentInfo& component_info = source_pool_components[i];
					
					void* source_ptr = source_pool->getComponent(entity, component_info.id);
					void* destination_ptr = destination_pool->getComponent(entity, component_info.id);

					memcpy_s(destination_ptr, component_info.size, source_ptr, component_info.size);
				}

				source_pool->removeEntity(entity);
				this->entity_map[entity] = new_entity_signature;

				//Instantates and returns the new component
				void* component_ptr = destination_pool->getComponent(entity, new_component_id);
				return new(component_ptr)Component(args...);

				//This is the more efficient copy algorithm, however, it will take longer to implement, so above is a less efficient placeholder
				/* Assume that an entity has components A,B,D and we want to add component C to it.
				*  The entity was laid out as ABD in memory (assuming they were registered in alphabetical order)
				*  After this operation, the entity should be laid out as ABCD. So this means there will be 2 memcpy's to the new entity pool, one for A,B and the other for D
				*  If C is the first or last component, only one memcpy is required.
				*/
				//void* source_ptr = source_pool->getEntityPtr(entity);
				//void* destination_ptr = destination_pool->getEntityPtr(entity);
			}

			GENESIS_ENGINE_ERROR("Component with ID:{} already exists on Entity:{}", new_component_id, entity);
			return (Component*)source_pool->getComponent(entity, new_component_id);
		}

	protected:
		friend class EntityRegistry;

		EntityWorld(ComponentRegistryInfo* component_registry_info, EntityRegistryInfo* entity_registry_info) { this->component_registry_info = component_registry_info; this->entity_registry_info = entity_registry_info; };
		~EntityWorld()
		{
			for (auto entity : this->entity_map)
			{
				this->entity_map.erase(entity.first);
			}

			for (auto pool : this->entity_pool_map)
			{
				delete pool.second;
			}
		}

		EntityPool* getEntityPool(EntitySignature pool_signature)
		{
			if (!has_value(this->entity_pool_map, pool_signature))
			{
				this->entity_pool_map[pool_signature] = new EntityPool(pool_signature, this->component_registry_info->component_info);
			}

			return this->entity_pool_map[pool_signature];
		}

		// Adds the entity to the world
		void addEntity(EntityId entity, EntitySignature entity_signature)
		{
			if (entity_signature.any())
			{
				EntityPool* pool = this->getEntityPool(entity_signature);
				pool->addEntity(entity);
			}

			this->entity_map.insert({entity, entity_signature });
		}

		// Removes the entity from the world
		void removeEntity(EntityId entity)
		{
			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->entity_map, entity), "Entity doesn't exist in this");
			EntitySignature entity_signature = this->entity_map[entity];
			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->entity_pool_map, entity_signature), "Entity Pool doesn't exist");
			EntityPool* pool = this->entity_pool_map[entity_signature];
			pool->removeEntity(entity);
			this->entity_map.erase(entity);
		}

		// Deconstructs the entity and removes it from the world
		void deleteEntity(EntityId entity)
		{
			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->entity_map, entity), "Entity doesn't exist in this");
			EntitySignature entity_signature = this->entity_map[entity];
			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->entity_pool_map, entity_signature), "Entity Pool doesn't exist");
			EntityPool* pool = this->entity_pool_map[entity_signature];
			pool->destroyEntity(entity);
			pool->removeEntity(entity);
			this->entity_map.erase(entity);
		}

		//Registry Info
		ComponentRegistryInfo* component_registry_info;
		EntityRegistryInfo* entity_registry_info;

		flat_hash_map<EntityId, EntitySignature> entity_map;
		flat_hash_map<EntitySignature, EntityPool*, hash<EntitySignature>> entity_pool_map;

		//Used to quickly access any Entity Pool that matches a given signature
		flat_hash_map<EntitySignature, vector<EntityPool*>> entity_pool_cache;
	};

	class EntityRegistry
	{
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
			GENESIS_ENGINE_ASSERT_ERROR(!has_value(this->component_registry_info.component_hash_to_id_map, hash), "Component Registered Already");
			ComponentId component_id = this->component_registry_info.next_component_id++;
			this->component_registry_info.component_hash_to_id_map[hash] = component_id;
			this->component_registry_info.component_info[component_id] = { component_id, size, deconstructor };
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

		template<class... Components>
		EntitySignature getSignature()
		{
			return this->component_registry_info.getSignature<Components...>();
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
			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->worlds, world), "World is not part of this Registry");
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

	protected:
		ComponentRegistryInfo component_registry_info;
		EntityRegistryInfo entity_registry_info;
		flat_hash_set<EntityWorld*> worlds;
	};
}
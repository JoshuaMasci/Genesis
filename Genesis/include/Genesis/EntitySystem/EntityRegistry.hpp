#pragma once

#include <vector>
#include <unordered_map>
#include <tsl/array_map.h>

#include "Genesis/EntitySystem/EntityTypes.hpp"
#include "Genesis/EntitySystem/EntitySignatureTable.hpp"
#include "Genesis/EntitySystem/EntityView.hpp"

#include "Genesis/Debug/Assert.hpp"

namespace Genesis
{
	namespace EntitySystem
	{
		class EntityRegistry
		{
		public:

			~EntityRegistry()
			{
				for (auto it = this->entity_map.begin(); it != this->entity_map.end(); ++it)
				{
					this->deconstructEntity(it->first);
				}

				for (auto it = this->entity_signature_table.begin(); it != this->entity_signature_table.end(); ++it)
				{
					delete it->second;
				}
			};

			template<class Component>
			ComponentId registerComponent()
			{
				return this->registerComponent<Component>([](void* ptr) { ((Component*)ptr)->~Component(); });
			};

			template<class Component>
			ComponentId registerComponent(std::function<void(void*)> delete_function)
			{
				size_t hash_value = EntitySystem::TypeInfo<Component>().getHash();
				GENESIS_ENGINE_ASSERT_ERROR((!has_value(this->component_map, hash_value)), "Component Registered Already");

				ComponentId id = (ComponentId)this->component_sizes.size();
				this->component_sizes.push_back(EntitySystem::TypeInfo<Component>().getSize());
				this->component_delete_function.push_back(delete_function);
				this->component_map[hash_value] = { id };

				return id;
			};

			template<class Component>
			ComponentId getComponentID()
			{
				size_t hash_value = EntitySystem::TypeInfo<Component>().getHash();
				GENESIS_ENGINE_ASSERT_ERROR((has_value(this->component_map, hash_value)), "Component Not Registered");
				return this->component_map[hash_value];
			};

			template<class Component>
			EntitySignature getSignature()
			{
				return EntitySignature(uint64_t(1) << this->getComponentID<Component>());
			};

			template<class Component1, class Component2, class... Components>
			EntitySignature getSignature()
			{
				return this->getSignature<Component1>() | this->getSignature<Component2, Components...>();
			};

			template<class... Components>
			EntityHandle createEntity()
			{
				EntitySignature entity_signature = this->getSignature<Components...>();

				EntityHandle entity = this->next_entity_id;
				this->entity_map[entity] = entity_signature;

				this->next_entity_id++;

				if (entity_signature == EntitySignature(0))
				{
					return entity;
				}


				//Create table if it doesn't exist
				if (!has_value(this->entity_signature_table, entity_signature))
				{
					this->entity_signature_table[entity_signature] = new EntitySystem::EntitySignatureTable(entity_signature, this->component_sizes);

					//Adds all signature to all caches that is fits
					for (auto element : this->entity_signature_cache)
					{
						if ((entity_signature & element.first) == element.first)
						{
							element.second.push_back(element.first);
						}
					}
				}

				this->entity_signature_table[entity_signature]->addEntity(entity);

				return entity;
			};

			void destroyEntity(EntityHandle entity)
			{
				GENESIS_ENGINE_ASSERT_ERROR((has_value(this->entity_map, entity)), "Entity does not exist");
				EntitySignature entity_signature = this->entity_map[entity];

				GENESIS_ENGINE_ASSERT_ERROR((has_value(this->entity_signature_table, entity_signature)), "Entity Signature Table does not exist");
				EntitySystem::EntitySignatureTable* table = this->entity_signature_table[entity_signature];

				for (ComponentId i = 0; i < (ComponentId)component_sizes.size(); i++)
				{
					if (entity_signature[i])
					{
						this->component_delete_function[i](table->getComponent(entity, i));
					}
				}

				table->removeEntity(entity);
				this->entity_map.erase(entity);
			};

			template<class Component, typename... Args>
			Component* initalizeComponent(EntityHandle entity, Args &&... args)
			{
				ComponentId component_id = this->getComponentID<Component>();

				GENESIS_ENGINE_ASSERT_ERROR((has_value(this->entity_map, entity)), "Entity does not exist");
				EntitySignature entity_signature = this->entity_map[entity];

				GENESIS_ENGINE_ASSERT_ERROR((has_value(this->entity_signature_table, entity_signature)), "Entity Signature Table does not exist");
				void* component_ptr = this->entity_signature_table[entity_signature]->getComponent(entity, component_id);

				return new(component_ptr)Component(args...);
			};


			template<class Component>
			Component* getComponent(EntityHandle entity)
			{
				ComponentId component_id = this->getComponentID<Component>();

				GENESIS_ENGINE_ASSERT_ERROR((has_value(this->entity_map, entity)), "Entity does not exist");
				EntitySignature entity_signature = this->entity_map[entity];

				GENESIS_ENGINE_ASSERT_ERROR((has_value(this->entity_signature_table, entity_signature)), "Entity Signature Table does not exist");
				return (Component*)this->entity_signature_table[entity_signature]->getComponent(entity, component_id);
			};

			template<class... Components>
			EntityView getExactView()
			{
				EntitySignature entity_signature = this->getSignature<Components...>();

				EntitySystem::EntitySignatureTable* table = nullptr;

				if (has_value(this->entity_signature_table, entity_signature))
				{
					table = this->entity_signature_table[entity_signature];
				}

				return EntityView(table, &this->component_map);
			};

			template<class... Components>
			std::vector<EntityView> getView()
			{
				EntitySignature entity_signature = this->getSignature<Components...>();

				//Create Cache for this signature
				if (!has_value(this->entity_signature_cache, entity_signature))
				{
					this->createEntitySignatureCache(entity_signature);
				}

				std::vector<EntitySignature>& signature_cache = this->entity_signature_cache[entity_signature];

				std::vector<EntityView> views(signature_cache.size());
				for (size_t i = 0; i < views.size(); i++)
				{
					views[i] = EntityView(this->entity_signature_table[signature_cache[i]], &this->component_map);
				}

				return views;
			};

		protected:

			//Calls the deconstrutor on all components
			//Used for fast world deletion
			void deconstructEntity(EntityHandle entity)
			{
				GENESIS_ENGINE_ASSERT_ERROR((has_value(this->entity_map, entity)), "Entity does not exist");
				EntitySignature entity_signature = this->entity_map[entity];

				GENESIS_ENGINE_ASSERT_ERROR((has_value(this->entity_signature_table, entity_signature)), "Entity Signature Table does not exist");
				EntitySystem::EntitySignatureTable* table = this->entity_signature_table[entity_signature];

				for (ComponentId i = 0; i < (ComponentId)component_sizes.size(); i++)
				{
					if (entity_signature[i])
					{
						this->component_delete_function[i](table->getComponent(entity, i));
					}
				}
			};

			//Used to create
			void createEntitySignatureCache(EntitySignature entity_signature)
			{
				std::vector<EntitySignature>& signature_cache = this->entity_signature_cache[entity_signature];

				//Adds all signatures that have the necessary components
				for (auto it = this->entity_signature_table.begin(); it != this->entity_signature_table.end(); ++it)
				{
					if ((it->first & entity_signature) == entity_signature)
					{
						signature_cache.push_back(it->first);
					}
				}
			};


			std::unordered_map<size_t, ComponentId> component_map;
			std::vector<size_t> component_sizes;
			std::vector <std::function<void(void*)>> component_delete_function;

			EntityHandle next_entity_id = 0;
			std::unordered_map<EntityHandle, EntitySignature> entity_map;

			std::unordered_map<EntitySignature, EntitySystem::EntitySignatureTable*> entity_signature_table;

			std::unordered_map<EntitySignature, std::vector<EntitySignature>> entity_signature_cache;
		};
	}
}
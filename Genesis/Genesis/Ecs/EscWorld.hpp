#pragma once

#include <vector>
#include <map>
#include <functional>

#include "Genesis/Ecs/EcsTypes.hpp"
#include "Genesis/Ecs/EntitySignatureTable.hpp"

#include "Genesis/Debug/Assert.hpp"

namespace Genesis
{
	class EcsWorld
	{
	public:

		~EcsWorld()
		{
			for (auto entity : this->entity_map)
			{
				this->deconstructEntity(entity.first);
			}

			for (auto table : this->entity_signature_table)
			{
				delete table.second;
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
			size_t hash_value = Ecs::TypeInfo<Component>().getHash();
			GENESIS_ENGINE_ASSERT_ERROR((!has_value(this->component_map, hash_value)), "Component Registered Already");

			ComponentId id = (ComponentId)this->component_sizes.size();
			this->component_sizes.push_back(Ecs::TypeInfo<Component>().getSize());
			this->component_delete_function.push_back(delete_function);
			this->component_map[hash_value] = { id };
			
			return id;
		};

		template<class Component>
		ComponentId getComponentID()
		{
			size_t hash_value = Ecs::TypeInfo<Component>().getHash();
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
			EntitySignature signature = this->getSignature<Components...>();

			EntityHandle entity = this->next_entity_id;
			this->entity_map[entity] = signature;

			this->next_entity_id++;

			if (signature == EntitySignature(0))
			{
				return entity;
			}

			if (!has_value(this->entity_signature_table, signature))
			{
				this->entity_signature_table[signature] = new Ecs::EntitySignatureTable(signature, this->component_sizes);
			}

			this->entity_signature_table[signature]->addEntity(entity);

			return entity;
		};

		void destroyEntity(EntityHandle entity)
		{
			GENESIS_ENGINE_ASSERT_ERROR((has_value(this->entity_map, entity)), "Entity does not exist");
			EntitySignature entity_signature = this->entity_map[entity];

			GENESIS_ENGINE_ASSERT_ERROR((has_value(this->entity_signature_table, entity_signature)), "Entity Signature Table does not exist");
			Ecs::EntitySignatureTable* table = this->entity_signature_table[entity_signature];

			for (ComponentId i = 0; i < (ComponentId)component_sizes.size(); i++)
			{
				if (entity_signature.get(i))
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
		}


		template<class Component>
		Component* getComponent(EntityHandle entity)
		{
			ComponentId component_id = this->getComponentID<Component>();

			GENESIS_ENGINE_ASSERT_ERROR((has_value(this->entity_map, entity)), "Entity does not exist");
			EntitySignature entity_signature = this->entity_map[entity];

			GENESIS_ENGINE_ASSERT_ERROR((has_value(this->entity_signature_table, entity_signature)), "Entity Signature Table does not exist");
			return (Component*)this->entity_signature_table[entity_signature]->getComponent(entity, component_id);
		};

	protected:

		//Same as Destory Entity, except it does not change the table
		//Used for fast world deletion
		void deconstructEntity(EntityHandle entity)
		{
			GENESIS_ENGINE_ASSERT_ERROR((has_value(this->entity_map, entity)), "Entity does not exist");
			EntitySignature entity_signature = this->entity_map[entity];

			GENESIS_ENGINE_ASSERT_ERROR((has_value(this->entity_signature_table, entity_signature)), "Entity Signature Table does not exist");
			Ecs::EntitySignatureTable* table = this->entity_signature_table[entity_signature];

			for (ComponentId i = 0; i < (ComponentId)component_sizes.size(); i++)
			{
				if (entity_signature.get(i))
				{
					this->component_delete_function[i](table->getComponent(entity, i));
				}
			}
		};


		std::map<size_t, ComponentId> component_map;
		std::vector<size_t> component_sizes;
		std::vector <std::function<void(void*)>> component_delete_function;

		EntityHandle next_entity_id = 0;
		std::map<EntityHandle, EntitySignature> entity_map;

		std::map<EntitySignature, Ecs::EntitySignatureTable*> entity_signature_table;
	};
}
#pragma once

#include "EntityTypes.hpp"
#include "BlockArray.hpp"

namespace Genesis
{
	class EntityPool
	{
	public:
		struct PoolComponentInfo
		{
			ComponentId id;
			size_t size;
			size_t offset;
			ComponentDeleteFunction delete_function;
		};

		EntityPool(EntitySignature signature, ComponentInfo registry_component_info[])
		{
			this->pool_signature = signature;

			size_t offset = 0;

			for (size_t i = 0; i < signature.size(); i++)
			{
				if (signature[i])
				{
					this->component_info.push_back({ registry_component_info[i].component_id, registry_component_info[i].component_size, offset, registry_component_info[i].component_delete_function });
					this->component_index_map[registry_component_info[i].component_id] = i;
					offset += registry_component_info[i].component_size;
				}
			}

			this->memory_block = new BlockArray(offset, MIN_BLOCK_COUNT);
		};

		~EntityPool() 
		{
			for (size_t entity_index = 0; entity_index < this->entities_index_to_handle.size(); entity_index++)
			{
				for (size_t component_index = 0; component_index < this->component_info.size(); component_index++)
				{
					this->component_info[component_index].delete_function(this->memory_block->getBlock(entity_index, this->component_info[component_index].offset));
				}
			}

			delete this->memory_block;
		};

		EntitySignature getSignature()
		{
			return this->pool_signature;
		};

		void addEntity(EntityId entity)
		{
			GENESIS_ENGINE_ASSERT_ERROR(!(has_value(this->entities_handle_to_index, entity)), "Entity is already in map");

			//Fill the Entity Tables
			size_t new_entity_index = this->entities_index_to_handle.size();
			this->entities_index_to_handle.push_back(entity);
			this->entities_handle_to_index[entity] = new_entity_index;

			//Check if memory needs resizing
			size_t current_block_count = this->memory_block->getBlockCount();
			if (new_entity_index >= current_block_count)
			{
				this->memory_block->resize(current_block_count * 2);
			}
		};

		void removeEntity(EntityId entity)
		{
			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->entities_handle_to_index, entity), "Entity not in map");
			size_t index_to_remove = this->entities_handle_to_index[entity];
			size_t index_last = this->entities_index_to_handle.size() - 1;

			//If entity is not the last one
			//Swap the last entity into it's slot
			if (index_to_remove != index_last)
			{
				this->memory_block->moveBlock(index_last, index_to_remove);
				this->entities_index_to_handle[index_to_remove] = this->entities_index_to_handle[index_last];
				this->entities_handle_to_index[this->entities_index_to_handle[index_to_remove]] = index_to_remove;
			}

			//Remove Last
			this->entities_index_to_handle.pop_back();
			this->entities_handle_to_index.erase(entity);

			//Check if table can be shrunk
			size_t current_entity_count = this->entities_index_to_handle.size();
			size_t current_block_count = this->memory_block->getBlockCount();
			if (current_block_count > (current_entity_count * GROWTH_RATE))
			{
				size_t new_size = std::max(current_block_count / 2, MIN_BLOCK_COUNT);
				this->memory_block->resize(new_size);
			}
		};

		//Calls the deconstructors for each components
		void destroyEntity(EntityId entity)
		{
			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->entities_handle_to_index, entity), "Entity not in map");

			size_t index_to_remove = this->entities_handle_to_index[entity];
			for (size_t component_index = 0; component_index < this->component_info.size(); component_index++)
			{
				this->component_info[component_index].delete_function(this->memory_block->getBlock(index_to_remove, this->component_info[component_index].offset));
			}
		};

		size_t getEntitySize()
		{
			return this->memory_block->getBlockSize();
		}

		void* getEntityPtr(EntityId entity)
		{
			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->entities_handle_to_index, entity), "Entity not in map");
			return this->memory_block->getBlock(this->entities_handle_to_index[entity], 0);
		}

		void* getComponent(EntityId entity, ComponentId component_id)
		{
			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->entities_handle_to_index, entity), "Entity Index does not exist in Entity Signature table");
			GENESIS_ENGINE_ASSERT_ERROR(has_value(this->component_index_map, component_id), "Component Offest does not exist in Entity Signature table");

			return this->memory_block->getBlock(this->entities_handle_to_index[entity], this->component_info[this->component_index_map[component_id]].offset);
		};

		vector<PoolComponentInfo>& getComponentInfo()
		{
			return this->component_info;
		};

	protected:
		const size_t GROWTH_RATE = 2;
		const size_t MIN_BLOCK_COUNT = 16;
		vector<PoolComponentInfo> component_info;
		flat_hash_map<ComponentId, size_t> component_index_map;

		EntitySignature pool_signature;

		vector<EntityId> entities_index_to_handle;
		flat_hash_map<EntityId, size_t> entities_handle_to_index;

		BlockArray* memory_block;
	};
}
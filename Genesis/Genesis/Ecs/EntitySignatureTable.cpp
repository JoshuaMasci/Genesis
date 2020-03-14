#include "EntitySignatureTable.hpp"

#include "Genesis/Core/Types.hpp"
#include "Genesis/Debug/Assert.hpp"

Genesis::Ecs::EntitySignatureTable::EntitySignatureTable(EntitySignature signature, std::vector<size_t>& component_sizes)
{
	this->table_signature = signature;

	size_t offset = 0;

	for (size_t i = 0; i < component_sizes.size(); i++)
	{
		if (signature.get(i))
		{
			this->component_offset[(ComponentId)i] = offset;
			offset += component_sizes[i];
		}
	}

	this->entity_memory = new MemoryArray(offset, 1);
}

Genesis::Ecs::EntitySignatureTable::~EntitySignatureTable()
{
	delete this->entity_memory;
}

void Genesis::Ecs::EntitySignatureTable::addEntity(EntityHandle entity)
{
	GENESIS_ENGINE_ASSERT_ERROR(!(has_value(this->entities_handle_to_index, entity)), "Entity is already in table");

	//Fill the Entity Tables
	size_t new_entity_index = this->entities_index_to_handle.size();
	this->entities_index_to_handle.push_back(entity);
	this->entities_handle_to_index[entity] = new_entity_index;

	//Check Memory for space, resize if needed
	if (new_entity_index > this->entity_memory->getBlockCount())
	{
		this->entity_memory->increaseSize(this->memory_step_size);
	}
}

void Genesis::Ecs::EntitySignatureTable::removeEntity(EntityHandle entity)
{
	GENESIS_ENGINE_ASSERT_ERROR((has_value(this->entities_handle_to_index, entity)), "Entity not in table");
	size_t index_to_remove = this->entities_handle_to_index[entity];
	size_t index_last = this->entities_index_to_handle.size() - 1;

	//If entity is not the last one
	//Swap the last entity into it's slot
	if (index_to_remove != index_last)
	{
		this->entity_memory->moveBlock(index_last, index_to_remove);
		this->entities_index_to_handle[index_to_remove] = this->entities_index_to_handle[index_last];
		this->entities_handle_to_index[this->entities_index_to_handle[index_to_remove]] = index_to_remove;
	}

	//Remove Last
	this->entities_index_to_handle.pop_back();
	this->entities_handle_to_index.erase(entity);
}

void* Genesis::Ecs::EntitySignatureTable::getComponent(EntityHandle entity, ComponentId component_id)
{
	GENESIS_ENGINE_ASSERT_ERROR((has_value(this->entities_handle_to_index, entity)), "Entity Index does not exist in Entity Signature table");
	GENESIS_ENGINE_ASSERT_ERROR((has_value(this->component_offset, component_id)), "Component Offest does not exist in Entity Signature table");

	return this->entity_memory->getBlock(this->entities_handle_to_index[entity], this->component_offset[component_id]);
}

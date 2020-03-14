#pragma once

#include <vector>
#include <map>

#include "Genesis/Ecs/EcsTypes.hpp"
#include "Genesis/Ecs/MemoryArray.hpp"

namespace Genesis
{
	namespace Ecs
	{
		class EntitySignatureTable
		{
		public:
			EntitySignatureTable(EntitySignature signature, std::vector<size_t>& component_sizes);
			~EntitySignatureTable();

			void addEntity(EntityHandle entity);
			void removeEntity(EntityHandle entity);

			void* getComponent(EntityHandle entity, ComponentId component_id);

		private:
			const size_t memory_step_size = 5;

			EntitySignature table_signature;

			std::vector<EntityHandle> entities_index_to_handle;
			std::map<EntityHandle, size_t> entities_handle_to_index;

			std::map<ComponentId, size_t> component_offset;
			MemoryArray* entity_memory;
		};
	}
}
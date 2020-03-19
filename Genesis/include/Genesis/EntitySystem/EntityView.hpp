#pragma once

#include <unordered_map>

#include "Genesis/EntitySystem/EntityTypes.hpp"
#include "Genesis/EntitySystem/EntitySignatureTable.hpp"

namespace Genesis
{
	namespace EntitySystem
	{
		class EntityView
		{
		public:
			EntityView(EntitySignatureTable* signature_table = nullptr, std::unordered_map<size_t, ComponentId>* component_map = nullptr)
			{
				this->signature_table = signature_table;
				this->component_map = component_map;
			};

			size_t getSize()
			{
				if (this->signature_table == nullptr)
				{
					return 0;
				}

				return this->signature_table->getSize();
			};

			EntityHandle get(size_t index)
			{
				GENESIS_ENGINE_ASSERT_ERROR((this->signature_table != nullptr), "View does not exist");
				return this->signature_table->getEntity(index);
			};

			template<class Component>
			Component* getComponent(size_t index)
			{
				GENESIS_ENGINE_ASSERT_ERROR((this->signature_table != nullptr), "View does not exist");
				return (Component*)this->signature_table->getComponentIndex(index, this->getComponentID<Component>());
			};

		private:

			template<class Component>
			ComponentId getComponentID()
			{
				size_t hash_value = TypeInfo<Component>().getHash();
				GENESIS_ENGINE_ASSERT_ERROR((has_value((*this->component_map), hash_value)), "Component Not Registered");
				return (*this->component_map)[hash_value];
			};

			EntitySignatureTable* signature_table;
			std::unordered_map<size_t, ComponentId>* component_map;
		};
	}
}
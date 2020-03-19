#pragma once

#include <vector>
#include "Genesis/EntitySystem/EntityTypes.hpp"
#include "Genesis/EntitySystem/EntityView.hpp"

#include "Genesis/Debug/Log.hpp"

namespace Genesis
{
	namespace EntitySystem
	{
		class EntitySystem
		{
		public:
			virtual EntitySignature getSystemSignature() = 0;
			virtual void runSystem(EntityView view) = 0;
		};

		class EntitySystemSceduler
		{
		public:
			EntitySystemSceduler(std::vector<EntitySystem*> systems) 
			{
			
			};


		protected:
			std::vector<EntitySystem*> systems;

			/*EntitySystemSceduler(_signatures)
			{
				std::vector<size_t> active_systems;

				for (size_t system_index = 0; system_index < system_signatures.size(); system_index++)
				{
					for (size_t active_index = 0; active_index < active_systems.size(); active_index++)
					{
						if ((system_signatures[system_index] & system_signatures[active_systems[active_index]]).any())
						{
							GENESIS_ENGINE_INFO("System Sync Point: {}", active_systems[active_index]);

							//Remove Job from pool
							active_systems[active_index] = active_systems[active_systems.size() - 1];
							active_systems.pop_back();
							active_index--;
						}
					}

					GENESIS_ENGINE_INFO("System Started: {}", system_index);
					active_systems.push_back(system_index);
				}

				for (size_t active_index = 0; active_index < active_systems.size(); active_index++)
				{
					GENESIS_ENGINE_INFO("System End Point: {}", active_systems[active_index]);
				}
			}*/
		};
	}
}
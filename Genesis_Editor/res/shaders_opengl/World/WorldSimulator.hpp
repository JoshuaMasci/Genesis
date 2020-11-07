#pragma once

namespace Genesis
{
	namespace Experimental
	{
		class Entity;
		class World;

		typedef void(*WorldUpdateFunction)(const TimeStep, World*);
		typedef void(*RootEntityUpdateFunction)(const TimeStep, World*, Entity*);
		typedef void(*EntityUpdateFunction)(const TimeStep, World*, Entity*, const TransformF&, Entity*);

		struct WorldSystem
		{
			WorldUpdateFunction pre_update;
			RootEntityUpdateFunction root_update;
			EntityUpdateFunction entity_update;
			WorldUpdateFunction post_update;
		};

		class WorldSimulator
		{
		public:
			void addWorldSystem(const WorldSystem& system);

			void simulateWorld(const TimeStep time_step, World* world);

		protected:
			void updateRootEntity(const TimeStep time_step, World* world, Entity* root);
			void updateEntity(const TimeStep time_step, World* world, Entity* root, const TransformF& parent_transform, Entity* entity);

			vector<WorldUpdateFunction> preWorldUpdateFunctions;
			vector<WorldUpdateFunction> postWorldUpdateFunctions;

			vector<RootEntityUpdateFunction> rootEntityUpdateFunctions;
			vector<EntityUpdateFunction> entityUpdateFunctions;

		};
	}
}
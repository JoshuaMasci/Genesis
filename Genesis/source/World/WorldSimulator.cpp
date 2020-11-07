#include "Genesis/World/WorldSimulator.hpp"

#include "Genesis/World/Entity.hpp"
#include "Genesis/World/World.hpp"

//World Components
#include "Genesis/Physics/PhysicsWorld.hpp"
#include "Genesis/Rendering/SceneInfo.hpp"

//Entity Components
#include "Genesis/Physics/RigidBody.hpp"

namespace Genesis
{
	namespace Experimental
	{

		void WorldSimulator::addWorldSystem(const WorldSystem& system)
		{
			if (system.pre_update != nullptr)
			{
				this->preWorldUpdateFunctions.push_back(system.pre_update);
			}

			if (system.post_update != nullptr)
			{
				this->postWorldUpdateFunctions.push_back(system.post_update);
			}

			if (system.root_update != nullptr)
			{
				this->rootEntityUpdateFunctions.push_back(system.root_update);
			}

			if (system.entity_update != nullptr)
			{
				this->entityUpdateFunctions.push_back(system.entity_update);
			}
		}

		void WorldSimulator::simulateWorld(const TimeStep time_step, World* world)
		{
			/*SceneInfo* scene_info = world->getComponent<SceneInfo>();
			if (scene_info != nullptr)
			{
				scene_info->clearBuffers();
			}*/

			for (WorldUpdateFunction pre_update : this->preWorldUpdateFunctions)
			{
				pre_update(time_step, world);
			}

			for (Entity* entity : world->getEntities())
			{
				updateRootEntity(time_step, world, entity);
			}

			for (WorldUpdateFunction post_update : this->postWorldUpdateFunctions)
			{
				post_update(time_step, world);
			}

			/*PhysicsWorld* physics_world = world->getComponent<PhysicsWorld>();
			if (physics_world != nullptr)
			{
				physics_world->simulate(time_step);
			}*/
		}

		void WorldSimulator::updateRootEntity(const TimeStep time_step, World* world, Entity* root)
		{
			for (RootEntityUpdateFunction root_update : this->rootEntityUpdateFunctions)
			{
				root_update(time_step, world, root);
			}

			TransformF root_transform;

			for (EntityUpdateFunction entity_update : this->entityUpdateFunctions)
			{
				entity_update(time_step, world, root, root_transform, root);
			}

			for (Entity* child : root->getChildren())
			{
				updateEntity(time_step, world, root, root_transform, child);
			}
		}

		void WorldSimulator::updateEntity(const TimeStep time_step, World* world, Entity* root, const TransformF& parent_transform, Entity* entity)
		{
			TransformF root_transform;
			TransformUtils::transformByInplace(root_transform, parent_transform, entity->getLocalTransform());

			for (EntityUpdateFunction entity_update : this->entityUpdateFunctions)
			{
				entity_update(time_step, world, root, root_transform, root);
			}

			for (Entity* child : entity->getChildren())
			{
				updateEntity(time_step, world, root, root_transform, child);
			}
		}
	}
}
#pragma once

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	//Prototype class
	class RootEntity;

	typedef uint32_t WorldId;

	class World
	{
	public:
		World();
		virtual ~World();

		void addGameObjectToWorld(RootEntity* entity);
		void removeGameObjectFromWorld(RootEntity* entity);

		void updatePreFrame(double delta_time);
		void updateTick(double delta_time);
		void updatePostFrame(double delta_time);

	protected:
		vector<RootEntity*> root_entities_in_world;
	};
};
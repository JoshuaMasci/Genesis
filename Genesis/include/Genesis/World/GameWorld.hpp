#pragma once

#include "Genesis/Core/VectorSet.hpp"

namespace Genesis
{
	class GameObject;

	class GameWorld
	{
	protected:
		bool enabled = false;
		GameObject* parent = nullptr;
		vector_set<GameObject*> game_objects;

	public:
		
		void update(const TimeStep& time_step);

		void add_object(GameObject* object);
		void remove_object(GameObject* object);

		vector_set<GameObject*>& getGameObjects() { return this->game_objects; };
	};
}
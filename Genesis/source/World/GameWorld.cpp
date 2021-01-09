#include "Genesis/World/GameWorld.hpp"

#include "Genesis/World/GameObject.hpp"

namespace Genesis
{
	void GameWorld::update(const TimeStep& time_step)
	{
		for (GameObject* child : this->game_objects)
		{
			child->update(time_step);
		}
	}

	void GameWorld::add_object(GameObject* object)
	{
		object->world = this;
		object->root = object;
		this->game_objects.insert(object);

		if (this->enabled)
		{
			object->add_to_world(this);
		}
	}

	void GameWorld::remove_object(GameObject* object)
	{
		if (this->enabled)
		{
			object->remove_from_world();
		}

		this->game_objects.erase(object);
		object->world = nullptr;
		object->root = nullptr;
	}
}
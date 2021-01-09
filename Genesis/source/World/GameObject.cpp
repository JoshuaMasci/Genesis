#include "Genesis/World/GameObject.hpp"

namespace Genesis
{
	GameObject::GameObject(const string& name)
	{
		this->name = name;
	}

	GameObject::~GameObject()
	{
		for (GameObject* child : this->children)
		{
			delete child;
		}

		for (auto behaviour : this->behaviour_map)
		{
			delete behaviour.second;
		}
	}

	void GameObject::add_to_world(GameWorld* world)
	{
		this->world = world;

		if (this->parent)
		{
			this->root = this->parent->root;
		}
		else
		{
			this->root = this;
		}
		
		//Behaviours
		for (auto behaviour : this->behaviour_map)
		{
			behaviour.second->on_enabled(this);
		}

		//Children
		for (GameObject* child : this->children)
		{
			child->add_to_world(this->world);
		}
	}

	void GameObject::remove_from_world()
	{
		//Children
		for (GameObject* child : this->children)
		{
			child->remove_from_world();
		}

		this->root = nullptr;
		this->world = nullptr;

		//Behaviours
		for (auto behaviour : this->behaviour_map)
		{
			behaviour.second->on_disable();
		}
	}

	void GameObject::update(const TimeStep& time_step)
	{
		if (!this->world)
		{
			return;
		}

		//Transform
		if (this->parent && this->parent->transform_changed)
		{
			this->transform_changed = true;
			TransformUtils::transformByInplace(this->root_transform, this->parent->root_transform, this->local_transform);
		}

		//Behaviours
		for (auto behaviour : this->behaviour_map)
		{
			behaviour.second->on_update(time_step);
		}

		//Children
		for (GameObject* child : this->children)
		{
			child->update(time_step);
		}
	}

	void GameObject::add_child(GameObject* child)
	{
		child->parent = this;
		this->children.insert(child);

		if (this->world)
		{
			child->add_to_world(this->world);
		}
	}

	void GameObject::remove_child(GameObject* child)
	{
		if (this->world)
		{
			child->remove_from_world();
		}

		this->children.erase(child);
		child->parent = nullptr;
	}

	bool GameObject::has(size_t type_id)
	{
		return this->behaviour_map.find(type_id) != this->behaviour_map.end();
	}

	Behaviour* GameObject::get(size_t type_id)
	{
		return this->behaviour_map.find(type_id)->second;
	}

	void GameObject::remove(size_t type_id)
	{
		Behaviour* behaviour = this->behaviour_map.find(type_id)->second;

		if (this->world)
		{
			behaviour->on_disable();
		}
		this->behaviour_map.erase(type_id);
		delete behaviour;
	}
}
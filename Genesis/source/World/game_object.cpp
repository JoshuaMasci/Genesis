#include "Genesis/World/game_object.hpp"

namespace Genesis
{
	GameObject::GameObject()
	{
	}

	GameObject::~GameObject()
	{
		for (GameObject* child : this->children)
		{
			delete child;
		}
	}

	void GameObject::enable()
	{
		//Assert if already true
		this->enabled = true;


		if (this->parent)
		{
			this->root = this->parent->root;
		}
		else
		{
			this->root = this;
		}
		//Behaviours

		//Children
		for (GameObject* child : this->children)
		{
			child->enable();
		}
	}

	void GameObject::disable()
	{
		//Children
		for (GameObject* child : this->children)
		{
			child->disable();
		}

		this->root = nullptr;

		//Behaviours

		//Assert if already false
		this->enabled = false;
	}

	void GameObject::update(const double& time_step)
	{
		if (!this->enabled)
		{
			return;
		}

		//Behaviours

		//Children
		for (GameObject* child : this->children)
		{
			child->enable();
		}
	}

	void GameObject::add_child(GameObject* child)
	{
		child->parent = this;
		this->children.insert(child);

		if (this->enabled)
		{
			child->enable();
		}
	}

	void GameObject::remove_child(GameObject* child)
	{
		if (child->enabled)
		{
			child->disable();
		}
		this->children.erase(child);

		child->parent = nullptr;
	}
}
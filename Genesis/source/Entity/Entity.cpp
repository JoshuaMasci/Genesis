#include "Genesis/Entity/Entity.hpp"

#include "Genesis/World/World.hpp"
#include "Genesis/Physics/ReactPhyscis.hpp"
#include "Genesis/Physics/Rigidbody.hpp"
#include "Genesis/Physics/CollisionComponent.hpp"

namespace Genesis
{
	Entity::Entity(EntityId id)
	{
		this->id = id;
		this->name = "Entity_" + std::to_string(id);

		this->root_node = new Node("Root_Node");
		this->root_node->root = this;
	}

	Entity::Entity(EntityId id, string name)
	{
		this->id = id;
		this->name = name;

		this->root_node = new Node("Root_Node");
		this->root_node->root = this;
	}

	Entity::~Entity()
	{
		delete this->root_node;
		delete this->rigidbody;
	}

	void Entity::onUpdate(TimeStep time_step)
	{
		this->root_node->onUpdate(time_step);
	}

	TransformD Entity::getWorldTransform()
	{
		if (this->rigidbody != nullptr && this->rigidbody->inWorld())
		{
			this->rigidbody->getTransform(this->world_transform);
		}

		return this->world_transform;
	}

	void Entity::setWorldTransform(const TransformD& transform)
	{
		vector3D old_scale = this->world_transform.getScale();

		this->world_transform = transform;

		if (this->rigidbody != nullptr && this->rigidbody->inWorld())
		{
			this->rigidbody->setTransform(this->world_transform);
		}
		
		//Scale has changed
		if (old_scale != transform.getScale())
		{
			this->root_node->updateTransform();
		}
	}

	void Entity::createRigidbody()
	{
		GENESIS_ENGINE_ASSERT_ERROR(rigidbody == nullptr, ("{}:{} already has a rigidbody", this->id, this->name));
		this->rigidbody = new RigidBody();

		if (this->world != nullptr)
		{
			this->world->getPhysicsWorld()->addEntity(this);
		}
	}

	void Entity::removeRigidbody()
	{
		GENESIS_ENGINE_ASSERT_ERROR(rigidbody != nullptr, ("{}:{} doesn't has a rigidbody", this->id, this->name));

		if (this->world != nullptr)
		{
			this->world->getPhysicsWorld()->removeEntity(this);
		}
	}

	void Entity::setSubworld(World* world)
	{
		GENESIS_ENGINE_ASSERT_ERROR(!this->hasSubworld(), ("{} already has subworld", this->name)); 
		this->subworld = world; 
		if (this->subworld != nullptr) 
		{ 
			this->subworld->parent_entity = this; 
		}
	}

	void Entity::addtoWorld(World* world)
	{
		GENESIS_ENGINE_ASSERT_ERROR(world != nullptr, ("{}:{} tried to join a null world", this->id, this->name));
		GENESIS_ENGINE_ASSERT_ERROR(this->world == nullptr, ("{}:{} already in a world", this->id, this->name));
		this->world = world;

		if (this->rigidbody != nullptr)
		{
			this->world->getPhysicsWorld()->addEntity(this);
		}
	}

	void Entity::removeFromWorld()
	{
		GENESIS_ENGINE_ASSERT_ERROR(this->world != nullptr, ("{}:{} not in a world", this->id, this->name));

		if (this->rigidbody != nullptr)
		{
			this->world->getPhysicsWorld()->removeEntity(this);
		}

		this->world = nullptr;
	}

	Node::Node(string name)
		:name(name)
	{

	}

	Node::~Node()
	{
		for (auto component : this->component_map)
		{
			delete component.second;
		}

		for (auto child : this->children)
		{
			delete child;
		}
	}

	void Node::onUpdate(TimeStep time_step)
	{
		for (auto component : this->component_map)
		{
			component.second->onUpdate(time_step);
		}

		for (auto child : this->children)
		{
			child->onUpdate(time_step);
		}
	}

	void Node::setLocalTransform(const TransformF& transform)
	{
		this->local_transform = transform;
		this->updateTransform();
	}

	TransformD Node::getGlobalTransform()
	{
		GENESIS_ENGINE_ASSERT_ERROR(this->root != nullptr, ("{} must be attached to entity to have global transform", this->name));

		//Don't apply scale since it is applied at the root node
		TransformD world_transform = this->root->getWorldTransform();
		world_transform.setScale(vector3D(1.0f));
		return TransformUtils::transformBy(world_transform, this->root_transform);
	}

	void Node::addChild(Node* child)
	{
		GENESIS_ENGINE_ASSERT_ERROR(child != nullptr, ("{} tried to add a null child", this->name));
		GENESIS_ENGINE_ASSERT_ERROR(child->parent == nullptr, ("{} already has a parent", child->name));

		child->parent = this;
		this->children.push_back(child);

		if (this->root != nullptr)
		{
			child->addToEntity();

			if (this->root->getWorld() != nullptr)
			{
				child->addtoWorld(this->root->getWorld());
			}
		}
	}

	void Node::removeChild(Node* child)
	{
		GENESIS_ENGINE_ASSERT_ERROR(child != nullptr, ("{} tried to remove a null child", this->name));

		for (size_t i = 0; i < this->children.size(); i++)
		{
			if (this->children[i] == child)
			{
				size_t last_index = this->children.size() - 1;
				if (i != last_index)
				{
					this->children[i] = this->children[last_index];
				}
				this->children.pop_back();

				if (this->root != nullptr)
				{
					child->removeFromEntity();

					if (this->root->getWorld() != nullptr)
					{
						child->removeFromWorld();
					}
				}

				child->parent = nullptr;
				return;
			}
		}
		GENESIS_ENGINE_ERROR("{} is not a child of {}", child->name, this->name);
	}

	void Node::createCollisionShape()
	{
		this->collision_shape = new CollisionComponent(this, nullptr);
	}

	void Node::removeCollisionShape()
	{
		delete this->collision_shape;
	}

	void Node::addtoWorld(World* world)
	{
		for (auto component : this->component_map)
		{
			component.second->addtoWorld(world);
		}

		for (auto child : this->children)
		{
			child->addtoWorld(world);
		}
	}

	void Node::removeFromWorld()
	{
		for (auto component : this->component_map)
		{
			component.second->removeFromWorld();
		}

		for (auto child : this->children)
		{
			child->removeFromWorld();
		}
	}

	void Node::updateTransform()
	{
		if (this->parent != nullptr)
		{
			TransformUtils::transformByInplace(this->root_transform, this->parent->root_transform, this->local_transform);
		}
		else if (this->parent == nullptr && this->root != nullptr)
		{
			//Only apply scale
			TransformF parent_transform;
			parent_transform.setScale((vector3F)this->root->getWorldTransform().getScale());
			TransformUtils::transformByInplace(this->root_transform, parent_transform, this->local_transform);
		}
		else
		{
			this->root_transform = this->root_transform;
		}

		for (auto child : this->children)
		{
			child->updateTransform();
		}
	}

	void Genesis::Node::addToEntity()
	{
		if (this->parent != nullptr)
		{
			this->root = this->parent->root;
		}

		for (auto child : this->children)
		{
			child->addToEntity();
		}
	}

	void Genesis::Node::removeFromEntity()
	{
		this->root = nullptr;

		for (auto child : this->children)
		{
			child->removeFromEntity();
		}
	}
}
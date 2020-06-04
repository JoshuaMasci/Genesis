#include "Genesis/Entity/Entity.hpp"

#include "Genesis/Entity/World.hpp"
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

	TransformD Entity::getWorldTransform()
	{
		if (this->rigidbody != nullptr && this->rigidbody->inWorld())
		{
			this->world_transform = this->rigidbody->getTransform();
		}

		return this->world_transform;
	}

	void Entity::setWorldTransform(const TransformD& transform)
	{
		this->world_transform = transform;

		if (this->rigidbody != nullptr && this->rigidbody->inWorld())
		{
			this->rigidbody->setTransform(this->world_transform);
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

	void Entity::onUpdate(TimeStep time_step)
	{
		this->root_node->onUpdate(time_step);
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

	/*void Entity::updateTransform(TransformDirtyFlag parent_flag)
	{
		TransformDirtyFlag entity_flag = TransformDirtyFlag::None;

		if (this->isRoot())
		{
			//Update rigidbody position
			if (this->rigidbody != nullptr && this->rigidbody->getHandle() != nullptr)
			{
				TransformD transform = this->rigidbody->getTransform();
				if (transform.getPosition() != this->local_transform.getPosition())
				{
					this->setLocalPosition(transform.getPosition());
				}

				if (transform.getOrientation() != this->local_transform.getOrientation())
				{
					this->setLocalOrientation(transform.getOrientation());
				}
			}

			if (this->transform_dirty)
			{
				this->global_transform = this->local_transform;
				entity_flag = TransformDirtyFlag::Global;
				this->transform_dirty = false;

				//if scale changed
				if (this->local_transform.getScale() != this->root_transform.getScale())
				{
					this->root_transform = TransformD(vector3D(0.0), quaternionD(1.0, 0.0, 0.0, 0.0), this->local_transform.getScale());

					//Force update of all root transforms
					entity_flag = TransformDirtyFlag::All;
				}
			}
		}
		else
		{
			if (this->transform_dirty || parent_flag == TransformDirtyFlag::All)
			{
				this->global_transform.transformByInplace(this->parent->global_transform, this->local_transform);
				this->root_transform.transformByInplace(this->parent->root_transform, this->local_transform);
				entity_flag = TransformDirtyFlag::All;
				this->transform_dirty = false;
			}
			else if (parent_flag == TransformDirtyFlag::Global)
			{
				this->global_transform.transformByInplace(this->parent->global_transform, this->local_transform);
				entity_flag = TransformDirtyFlag::Global;
			}
		}

		if (entity_flag != TransformDirtyFlag::None)
		{
			this->onTransformUpdate(entity_flag);
		}

		for (Entity* child : this->children)
		{
			child->updateTransform(entity_flag);
		}
	}

	void Entity::onTransformUpdate(TransformDirtyFlag transform_dirty)
	{
		//GENESIS_ENGINE_INFO("{}:{} transform dirty {}", this->id, this->name, transform_dirty);

		for (auto component : this->component_map)
		{
			component.second->onTransformUpdate();
		}

		if (this->rigidbody != nullptr)
		{
			this->rigidbody->setTransform(this->local_transform);
		}
	}*/

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

	void Node::setLocalTransform(const TransformF& transform)
	{
		this->local_transform = transform;
		this->updateTransform();
	}

	TransformD Node::getGlobalTransform()
	{
		GENESIS_ENGINE_ASSERT_ERROR(this->root != nullptr, ("{} must be attached to entity to have global transform", this->name));
		return TransformUtils::transformBy(this->root->getWorldTransform(), this->root_transform);
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
		else
		{
			this->root_transform = this->local_transform;
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